#include "mqttSubscriber.h"

#include <mosquitto.h>
#include <map>
#include <vector>
#include <string>

using namespace std;

class MqttClient {
    protected:
        string id;
        string host;
        int port;
        int keepAlive;
        string user;
        string pass;
        struct mosquitto* mosquittoClient;
        multimap<string, MqttSubscriber*> subscriberMap;

    public:
        MqttClient(char* id, char* host, int port = 1883, int keepAlive = 60);
        void addSubscriber(MqttSubscriber* subscriber);
        vector<MqttSubscriber*> getSubscribers(string topic);
        string getId();

};

extern map<mosquitto*, MqttClient*> clientMap;