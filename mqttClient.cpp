#include "mqttClient.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>

map<mosquitto*, MqttClient*> clientMap;

void onMessageReceived(struct mosquitto *client, void *obj, const struct mosquitto_message *message){
    MqttClient* mqttClient = clientMap.at(client);

    for(MqttSubscriber* subscriber : mqttClient->getSubscribers(message->topic)) {
        subscriber->onMessage(message);
        LOG_INFO(
            "%s received a message on subscriber %s: %s", 
            mqttClient->getId().c_str(), 
            subscriber->getId().c_str(), 
            (const char*)message->payload);
    }
}

MqttClient::MqttClient(const char* id, const char* host, int port, int keepAlive){
    if(id != NULL)
        this->id = id;
    this->host = host;

    // Create new client.
    mosquittoClient = mosquitto_new(id, true, NULL);

    mosquitto_log_callback_set(mosquittoClient, [](mosquitto *mosq, void *obj, int level, const char *str){
        switch(level){
            case MOSQ_LOG_INFO:
            case MOSQ_LOG_NOTICE:
            LOG_INFO("MOSQUITTO: %s", str);
            break;
            case MOSQ_LOG_WARNING:
            LOG_WARN("MOSQUITTO: %s", str);
            break;
            case MOSQ_LOG_ERR:
            LOG_ERROR("MOSQUITTO: %s", str);
            break;
            case MOSQ_LOG_DEBUG:
            LOG_DEBUG("MOSQUITTO: %s", str);
            break;
        }
    });

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
            exit(-1);
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