#include "mqttSubscriber.h"
#include "log.h"

MqttSubscriber::MqttSubscriber(string id, vector<string> topics, vector<string> destinationPaths, bool createDestIfNotExists) {
    this->id = id;
    this->topics = topics;
    for(string path : destinationPaths) {
        destinations.push_back(new Tty(path.c_str()));
    }
}

void MqttSubscriber::onMessage(const struct mosquitto_message *message) {
    for(Tty* tty : destinations) {
        tty->type((char*)message->payload);
        if(insertNewLine)
            tty->type("\n");
    }
}

string MqttSubscriber::getId(){
    return id;
}

vector<string> MqttSubscriber::getTopics(){
    return topics;
}

void MqttSubscriber::setInsertNewLine(bool insertNewLine){
    LOG_INFO("%s", insertNewLine ? "true" : "false");
    this->insertNewLine = insertNewLine;
}