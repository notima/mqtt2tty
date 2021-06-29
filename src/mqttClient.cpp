#include "mqttClient.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>

map<mosquitto*, MqttClient*> clientMap;

void onMessageReceived(struct mosquitto *client, void *obj, const struct mosquitto_message *message){
    MqttClient* mqttClient = clientMap.at(client);

    for(MqttSubscriber* subscriber : mqttClient->getSubscribers(message->topic)) {
        LOG_INFO(
            "%s received a message on subscriber %s: %s", 
            mqttClient->getId().c_str(), 
            subscriber->getId().c_str(), 
            (const char*)message->payload);
        subscriber->onMessage(message);
    }
}

MqttClient::MqttClient(const char* id, const char* host, int port, int keepAlive){
    if(id != NULL)
        this->id = id;
    this->host = host;
    this->port = port;
    this->keepAlive = keepAlive;

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

    mosquitto_message_callback_set(mosquittoClient, onMessageReceived);
    clientMap.insert(pair<mosquitto*, MqttClient*>(mosquittoClient, this));
}

void MqttClient::connect(){
    LOG_DEBUG("\nHost: %s\nPort: %d\nKeep alive: %d",
            host.c_str(), port, keepAlive);
    // Connect
    int result = mosquitto_connect(mosquittoClient, host.c_str(), port, keepAlive);
    if (result == MOSQ_ERR_INVAL) {
        LOG_ERROR("Error connecting. The client might not be correctly set up");
        exit(-1);
    }
    if(result == MOSQ_ERR_ERRNO) {
        LOG_ERROR("Error connecting (%d)", errno);
        exit(-1);
    }
}

void MqttClient::addSubscriber(MqttSubscriber* subscriber){
    for(string topic: subscriber->getTopics()) {
        if(mosquitto_subscribe(mosquittoClient, NULL, topic.c_str(), 1)){
            LOG_ERROR("Subscription failed");
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

struct mosquitto* MqttClient::getMosquittoClient(){
    return mosquittoClient;
}