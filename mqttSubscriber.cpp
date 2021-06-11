#include "mqttSubscriber.h"

MqttSubscriber::MqttSubscriber(char* id, vector<string> topics, vector<string> destinationPaths, bool createDestIfNotExists) {
    this->id = id;
    this->topics = topics;
    for(string path : destinationPaths) {
        destinations.push_back(new Tty(path.c_str()));
    }
}

void MqttSubscriber::onMessage(const struct mosquitto_message *message) {
    for(Tty* tty : destinations) {
        tty->type((char*)message->payload);
    }
}

vector<string> MqttSubscriber::getTopics(){
    return topics;
}