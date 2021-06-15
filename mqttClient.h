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
        struct mosquitto* mosquittoClient;
        multimap<string, MqttSubscriber*> subscriberMap;

    public:
        MqttClient(const char* id, const char* host, int port = 1883, int keepAlive = 60);
        void connect();
        void addSubscriber(MqttSubscriber* subscriber);
        vector<MqttSubscriber*> getSubscribers(string topic);
        string getId();
        struct mosquitto* getMosquittoClient();
};

extern map<mosquitto*, MqttClient*> clientMap;