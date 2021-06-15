#include "tty.h"

#include <string>
#include <vector>
#include <mosquitto.h>

using namespace std;

/**
 * MqttSubscriber is a class that subscribes to one or many
 * MQTT topics and processes the messages received by forwarding
 * them to one or many pseudo terminals (destinations).
 */
class MqttSubscriber {
    protected:
        string id;
        bool insertNewLine = false;
        vector<string> topics;
        vector<Tty*> destinations;

    public:
        MqttSubscriber(string id, vector<string> topics, vector<string> destinationPaths, bool pseudo = false);
        void onMessage(const struct mosquitto_message *message);
        string getId();
        vector<string> getTopics();
        void setInsertNewLine(bool insertNewLine);
        void setTtyBaudRate(int baudRate);
};