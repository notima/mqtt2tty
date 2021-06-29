#include "mqttSubscriber.h"
#include "log.h"
#include "pty.h"

MqttSubscriber::MqttSubscriber(string id, vector<string> topics, vector<string> destinationPaths, bool pseudo) {
    this->id = id;
    this->topics = topics;
    for(string path : destinationPaths) {
        Tty* tty = pseudo ? new Pty(path.c_str()) : new Tty(path.c_str());
        tty->openTty();
        destinations.push_back(tty);
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
    this->insertNewLine = insertNewLine;
}

void MqttSubscriber::setTtyBaudRate(int baudRate) {
    for(Tty* tty : destinations) {
        tty->setBaudRate(baudRate);
    }
}