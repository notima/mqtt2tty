#include <iostream>
#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include <jansson_config.h>
#include <unistd.h>
#include <signal.h>
#include <libconfig.h>
#include "tty.h"
#include "mqttClient.h"

using namespace std;

struct mosquitto *client;

char* serialCpy;

json_t *server, *topic;
json_t *serialPort;

char config_file[] = "default.json";

char topicStr[255];

unsigned int baudRate = 115200;

Tty *tty;

#define BUFSIZE 2048

auto onExit = [] (int i) { 
    cout << endl;
    exit(0);
};

void on_message1(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    //cout << "Got a message\n";
    //cout << (char*)(message->payload) << endl;
    //tty->type((char*)(message->payload));
}  

static int readConfigurationFile(char* filename) {

    json_t *root, *data;
    json_error_t error;

    char buff[FILENAME_MAX];
    char pathChar[] = "/";
    getcwd(buff, FILENAME_MAX);

    char* newPath = NULL;
    char* finalPath = NULL;

    if (filename[0] != '/') {
        newPath = strcat(buff, pathChar);
        finalPath = strcat(newPath, filename);
    }

    root = json_load_file(finalPath, 0, &error);

    // free(finalPath);

    server = json_object_get(root, "server");
    if(!json_is_string(server))
    {
        fprintf(stderr, "error: server is not a string\n");
        json_decref(root);
        return 1;
    }

    topic = json_object_get(root, "topic");
    if(!json_is_string(topic))
    {
        fprintf(stderr, "error: topic is not a string\n");
        json_decref(root);
        return 1;
    }

    serialPort = json_object_get(root, "serialPort");
    if (!json_is_string(serialPort)) {
        fprintf(stderr, "error: serialPort is not a string\n");
        json_decref(root);
        return 1;
    }

    data = json_object_get(root, "baudRate");
    if (data) {
        baudRate = json_integer_value(data);
    }

    return  0;

}

void loop() {
    while(true){
        for(pair<mosquitto*, MqttClient*> client : clientMap){
            mosquitto_loop(client.first, -1, 1);
        }
    }
}

int main(int argc, char** argv)  {

    // Make sure onExit is run whenever the program is stopped.

    //^C
    signal(SIGINT, onExit);
    //abort()
    signal(SIGABRT, onExit);
    //sent by "kill" command
    signal(SIGTERM, onExit);
    //^Z
    signal(SIGTSTP, onExit);

    // Initialize Mosquitto library
    mosquitto_lib_init();

    int major;
    int minor;
    int revision;

    mosquitto_lib_version(&major, &minor, &revision);
    printf("Mosquitto library version %d.%d.%d\n", major, minor, revision);

    char serialBuf[BUFSIZE];

    std::cout << argv[0];
    if (argc==1) {
        readConfigurationFile(config_file);
    } else {
        readConfigurationFile(argv[1]);
    }

    speed_t baud = baudRate;

    MqttClient* client = new MqttClient("client 1", "localhost");
    MqttSubscriber* subscriber = new MqttSubscriber("sub 1", {"t1", "t2"}, {"/dev/ttyMQTT_t1"});
    client->addSubscriber(subscriber);

    MqttClient* client2 = new MqttClient("client 2", "localhost");
    MqttSubscriber* subscriber2 = new MqttSubscriber("sub 2", {"t2", "t3"}, {"/dev/ttyMQTT_t2"});
    client2->addSubscriber(subscriber2);

    loop();

    //mosquitto_loop_forever(client, -1, 1);

    //mosquitto_destroy(client);
    
    //osquitto_lib_cleanup();
    
    //printf("Result %d\n", result);
}
