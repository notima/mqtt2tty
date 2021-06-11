#include "mqttClient.h"

#include <stdio.h>
#include <stdlib.h>

map<mosquitto*, MqttClient*> clientMap;

void onMessageReceived(struct mosquitto *client, void *obj, const struct mosquitto_message *message){
    MqttClient* mqttClient = clientMap.at(client);

    for(MqttSubscriber* subscriber : mqttClient->getSubscribers(message->topic)) {
        subscriber->onMessage(message);
        char msg[255];
        sprintf( msg,
            "%s received a message on subscriber %s: %s", 
            mqttClient->getId().c_str(), 
            message->topic, 
            (const char*)message->payload);
        puts(msg);
    }
}

MqttClient::MqttClient(char* id, char* host, int port, int keepAlive){
    this->id = id;
    this->host = host;

    // Create new client.
    mosquittoClient = mosquitto_new(id, true, NULL);

    // Connect
    int result = mosquitto_connect(mosquittoClient, host, 1883, 60);
    if (result!=MOSQ_ERR_SUCCESS) {
        perror("Error connecting");
        exit(-1);
    }

    mosquitto_message_callback_set(mosquittoClient, onMessageReceived);

    clientMap.insert(pair<mosquitto*, MqttClient*>(mosquittoClient, this));
}

void MqttClient::addSubscriber(MqttSubscriber* subscriber){
    for(string topic: subscriber->getTopics()) {
        if(mosquitto_subscribe(mosquittoClient, NULL, topic.c_str(), 1)){
            perror("Subscription failed");
        }else{
            subscriberMap.insert(pair<string, MqttSubscriber*>(topic, subscriber));
        }
    }
}

vector<MqttSubscriber*> MqttClient::getSubscribers(string topic){
    vector<MqttSubscriber*> result;
    for (auto itr = subscriberMap.begin(); itr != subscriberMap.end(); itr++)    
        if (itr -> first == topic)        
            result.push_back(itr -> second);
    return result;
}

string MqttClient::getId() {
    return id;
}