/*  
  OpenMQTTGateway  - ESP8266 or Arduino program for home automation 

   Act as a wifi or ethernet gateway between your 433mhz/infrared IR signal  and a MQTT broker 
   Send and receiving command by MQTT
 
  This gateway enables to:
 - receive MQTT data from a topic and send RF 433Mhz signal corresponding to the received MQTT data
 - publish MQTT data to a different topic related to received 433Mhz signal

    Copyright: (c)Florian ROBERT
  
    This file is part of OpenMQTTGateway.
    
    OpenMQTTGateway is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenMQTTGateway is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifdef ZgatewayRF

#include <RCSwitch.h> // library for controling Radio frequency switch

RCSwitch mySwitch = RCSwitch();

void setupRF(){

  //RF init parameters
  mySwitch.enableTransmit(RF_EMITTER_PIN);
  trc(F("RF_EMITTER_PIN "));
  trc(RF_EMITTER_PIN);
  mySwitch.setRepeatTransmit(RF_EMITTER_REPEAT); 
  mySwitch.enableReceive(RF_RECEIVER_PIN); 
  trc(F("RF_RECEIVER_PIN "));
  trc(RF_RECEIVER_PIN);
  trc(F("ZgatewayRF setup done "));
}

void RFtoMQTT(){

  if (mySwitch.available()){
    trc(F("Creating RF buffer"));
    StaticJsonBuffer<JSON_MSG_BUFFER> jsonBuffer;
    JsonObject& RFdata = jsonBuffer.createObject();
    trc(F("Rcv. RF"));
    #ifdef ESP32
      String taskMessage = "RF Task running on core ";
      taskMessage = taskMessage + xPortGetCoreID();
      trc(taskMessage);
    #endif
    RFdata.set("value", (unsigned long)mySwitch.getReceivedValue());
    RFdata.set("protocol",(int)mySwitch.getReceivedProtocol());
    RFdata.set("length", (int)mySwitch.getReceivedBitlength());
    RFdata.set("delay", (int)mySwitch.getReceivedDelay());
    mySwitch.resetAvailable();
    
    trc(F("LED MNG"));
    digitalWrite(led_receive, LOW);
    timer_led_receive = millis();
    
    unsigned long MQTTvalue = RFdata.get<unsigned long>("value");
    if (!isAduplicate(MQTTvalue) && MQTTvalue!=0) {// conditions to avoid duplications of RF -->MQTT
        trc(F("Adv data RFtoMQTT")); 
        pub(subjectRFtoMQTT,RFdata);
        if (repeatRFwMQTT){
            trc(F("Publish RF for repeat"));
            pub(subjectMQTTtoRF,RFdata);
        }
    } 
  }
}

#ifdef simplePublishing
void MQTTtoRF(char * topicOri, char * datacallback) {

  unsigned long data = strtoul(datacallback, NULL, 10); // we will not be able to pass values > 4294967295

  // RF DATA ANALYSIS
  //We look into the subject to see if a special RF protocol is defined 
  String topic = topicOri;
  int valuePRT = 0;
  int valuePLSL  = 0;
  int valueBITS  = 0;
  int pos = topic.lastIndexOf(RFprotocolKey);       
  if (pos != -1){
    pos = pos + +strlen(RFprotocolKey);
    valuePRT = (topic.substring(pos,pos + 1)).toInt();
    trc(F("RF Protocol:"));
    trc(valuePRT);
  }
  //We look into the subject to see if a special RF pulselength is defined 
  int pos2 = topic.lastIndexOf(RFpulselengthKey);
  if (pos2 != -1) {
    pos2 = pos2 + strlen(RFpulselengthKey);
    valuePLSL = (topic.substring(pos2,pos2 + 3)).toInt();
    trc(F("RF Pulse Lgth:"));
    trc(valuePLSL);
  }
  int pos3 = topic.lastIndexOf(RFbitsKey);       
  if (pos3 != -1){
    pos3 = pos3 + strlen(RFbitsKey);
    valueBITS = (topic.substring(pos3,pos3 + 2)).toInt();
    trc(F("Bits nb:"));
    trc(valueBITS);
  }
  
  if ((topic == subjectMQTTtoRF) && (valuePRT == 0) && (valuePLSL  == 0) && (valueBITS == 0)){
    trc(F("MQTTtoRF dflt"));
    mySwitch.setProtocol(1,350);
    mySwitch.send(data, 24);
    // Acknowledgement to the GTWRF topic
    pub(subjectGTWRFtoMQTT, datacallback);  
  } else if ((valuePRT != 0) || (valuePLSL  != 0)|| (valueBITS  != 0)){
    trc(F("MQTTtoRF usr par."));
    if (valuePRT == 0) valuePRT = 1;
    if (valuePLSL == 0) valuePLSL = 350;
    if (valueBITS == 0) valueBITS = 24;
    trc(valuePRT);
    trc(valuePLSL);
    trc(valueBITS);
    mySwitch.setProtocol(valuePRT,valuePLSL);
    mySwitch.send(data, valueBITS);
    // Acknowledgement to the GTWRF topic 
    pub(subjectGTWRFtoMQTT, datacallback);// we acknowledge the sending by publishing the value to an acknowledgement topic, for the moment even if it is a signal repetition we acknowledge also
  } 
}
#endif

#ifdef jsonPublishing
  void MQTTtoRF(char * topicOri, JsonObject& RFdata) { // json object decoding
  
    String topic = topicOri;
  
    if (topic == subjectMQTTtoRF) {
      trc(F("MQTTtoRF json data analysis"));
      unsigned long data = RFdata["value"];
      if (data != 0) {
        trc(F("MQTTtoRF data ok"));
        int valuePRT =  RFdata["protocol"];
        int valuePLSL = RFdata["delay"];
        int valueBITS = RFdata["length"];
        if ((valuePRT == 0) && (valuePLSL  == 0) && (valueBITS == 0)){
          trc(F("MQTTtoRF dflt"));
          mySwitch.setProtocol(1,350);
          mySwitch.send(data, 24);
          // Acknowledgement to the GTWRF topic
          pub(subjectGTWRFtoMQTT, RFdata);  
        } else if ((valuePRT != 0) || (valuePLSL  != 0)|| (valueBITS  != 0)){
          trc(F("MQTTtoRF usr par."));
          if (valuePRT == 0) valuePRT = 1;
          if (valuePLSL == 0) valuePLSL = 350;
          if (valueBITS == 0) valueBITS = 24;
          trc(valuePRT);
          trc(valuePLSL);
          trc(valueBITS);
          mySwitch.setProtocol(valuePRT,valuePLSL);
          mySwitch.send(data, valueBITS);
          // Acknowledgement to the GTWRF topic 
          pub(subjectGTWRFtoMQTT, RFdata);// we acknowledge the sending by publishing the value to an acknowledgement topic, for the moment even if it is a signal repetition we acknowledge also
        } 
      }else{
        trc(F("MQTTtoRF Fail reading from json"));
      }
    }
  }
#endif
#endif
