#include "tty.h"

#include <string>
#include <vector>
#include <mosquitto.h>

using namespace std;

class MqttSubscriber {
    protected:
        string id;
        vector<string> topics;
        vector<Tty*> destinations;

    public:
        MqttSubscriber(char* id, vector<string> topics, vector<string> destinationPaths, bool createDestIfNotExists = true);
        void onMessage(const struct mosquitto_message *message);
        string getId();
        vector<string> getTopics();
};