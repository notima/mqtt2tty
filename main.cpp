#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <libconfig.h>
#include "tty.h"
#include "mqttClient.h"
#include "log.h"

using namespace std;

log4cpp::Category& root = log4cpp::Category::getRoot();
char log_buff[512];
char err_buff[512];

char config_file[] = "/home/oliver/Documents/mqtt2serial/build/default.cfg";

auto onExit = [] (int i) { 
    cout << endl;
    map<mosquitto*, MqttClient*>::iterator it;
    for (it = clientMap.begin(); it != clientMap.end(); it++) {
        mosquitto_destroy(it->first);
    }
    mosquitto_lib_cleanup();
    exit(0);
};

char *randomString(int length) {    
    static int mySeed = 25011984;
    const char *string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t stringLen = strlen(string);        
    char *randomString = NULL;

    srand(time(NULL) * length + ++mySeed);

    if (length < 1) {
        length = 1;
    }

    randomString = (char*)malloc(sizeof(char) * (length +1));

    if (randomString) {
        short key = 0;

        for (int n = 0;n < length;n++) {            
            key = rand() % stringLen;          
            randomString[n] = string[key];
        }

        randomString[length] = '\0';

        return randomString;        
    }
    else {
        printf("No memory");
        exit(1);
    }
}

static void initSubscribers(config_t cfg, MqttClient* client, config_list_t* subscriberList){
    for(int i = 0; i < subscriberList->length; i++){
        config_setting_t* subscriberCfg = subscriberList->elements[i];
        const char* id;
        const char* topic;
        vector<string> topics;
        const char* destination;
        vector<string> destinations;

        if(config_setting_lookup_string(subscriberCfg, "id", &id) == CONFIG_FALSE){
            char subName[26] = "sub-";
            id = strcat(subName, randomString(18));
        }

        if(config_setting_lookup_string(subscriberCfg, "topic", &topic)){
            topics.push_back(topic);
        }
        config_setting_t* topicList = config_setting_lookup(subscriberCfg, "topics");
        if(topicList)
            for(int i = 0; i < topicList->value.list->length; i++)
                topics.push_back(topicList->value.list->elements[i]->value.sval);
        
        if(config_setting_lookup_string(subscriberCfg, "destination", &destination)){
            destinations.push_back(destination);
        }
        config_setting_t* destinationList = config_setting_lookup(subscriberCfg, "destinations");
        if(destinationList)
            for(int i = 0; i < destinationList->value.list->length; i++)
                destinations.push_back(destinationList->value.list->elements[i]->value.sval);
        
        int pseudo = CONFIG_FALSE;
        config_setting_lookup_bool(subscriberCfg, "createPTY", &pseudo);

        LOG_INFO("Creating subscriber %s.", id);
        MqttSubscriber* subscriber = new MqttSubscriber(id, topics, destinations, pseudo == CONFIG_TRUE);

        int insertNewLine = CONFIG_FALSE;
        if(config_setting_lookup_bool(subscriberCfg, "insertNewLine", &insertNewLine)){
            subscriber->setInsertNewLine(insertNewLine == CONFIG_TRUE);
        }

        int baudRate;
        if(config_setting_lookup_int(subscriberCfg, "baud", &baudRate)){
            subscriber->setTtyBaudRate(baudRate);
        }

        client->addSubscriber(subscriber);
    }
}

static void initClients(config_t cfg, config_list_t* clientList){
    LOG_INFO("Initializing %d client(s)...", clientList->length);
    for(int i = 0; i < clientList->length; i++){
        config_setting_t* clientCfg = clientList->elements[i];
        const char* id;
        const char* host;
        int port = 1883;
        int keepAlive = 60;
        if(config_setting_lookup_string(clientCfg, "id", &id) == CONFIG_FALSE){
            char clientName[26] = "client-";
            id = strcat(clientName, randomString(18));
        }
        config_setting_lookup_string(clientCfg, "host", &host);
        config_setting_lookup_int(clientCfg, "port", &port);
        config_setting_lookup_int(clientCfg, "keepAlive", &keepAlive);
        MqttClient* client = new MqttClient(id, host, port, keepAlive);

        const char* username;
        const char* password;
        if(
            config_setting_lookup_string(clientCfg, "username", &username) == CONFIG_TRUE && 
            config_setting_lookup_string(clientCfg, "password", &password) == CONFIG_TRUE
        ){
            mosquitto_username_pw_set(client->getMosquittoClient(), username, password);
        }

        config_setting_t* tlsSettings = config_setting_lookup(clientCfg, "tls");
        if(tlsSettings){
            const char* caFile;
            const char* caPath;
            const char* certFile;
            const char* keyFile;
            config_setting_lookup_string(tlsSettings, "caFile", &caFile);
            config_setting_lookup_string(tlsSettings, "caPath", &caPath);
            config_setting_lookup_string(tlsSettings, "certFile", &certFile);
            config_setting_lookup_string(tlsSettings, "keyFile", &keyFile);
            mosquitto_tls_set(
                client->getMosquittoClient(), 
                caFile, 
                caPath, 
                certFile, 
                keyFile, 
                [](char *buf, int size, int rwflag, void *userdata) -> int {
                    //TODO: write the password to buf and return length of password.
                    LOG_WARN("TLS certificate requires password. This is not yet supported");
                    return 0;
                }
            );

            config_setting_t* tlsOptsSettings = config_setting_lookup(tlsSettings, "tls");
            if(tlsOptsSettings){
                int certReqs;
                const char* tlsVersion;
                const char* ciphers;
                config_setting_lookup_int(tlsOptsSettings, "certReqs", &certReqs);
                config_setting_lookup_string(tlsOptsSettings, "tlsVersion", &tlsVersion);
                config_setting_lookup_string(tlsOptsSettings, "ciphers", &ciphers);
                mosquitto_tls_opts_set(
                    client->getMosquittoClient(),
                    certReqs,
                    tlsVersion,
                    ciphers
                );
            }

            int insecure;
            if(config_setting_lookup_bool(tlsSettings, "insecure", &insecure)){
                mosquitto_tls_insecure_set(client->getMosquittoClient(), insecure == CONFIG_TRUE);
            }

            config_setting_t* tlsPskSettings = config_setting_lookup(tlsSettings, "psk");
            if(tlsPskSettings){
                const char* psk;
                const char* identity;
                const char* ciphers;
                config_setting_lookup_string(tlsPskSettings, "psk", &psk);
                config_setting_lookup_string(tlsPskSettings, "identity", &identity);
                config_setting_lookup_string(tlsPskSettings, "ciphers", &ciphers);
                mosquitto_tls_psk_set(
                    client->getMosquittoClient(),
                    psk,
                    identity,
                    ciphers
                );
            }
        }

        client->connect();
        config_setting_t* subscriberList = config_setting_lookup(clientCfg, "subscribers");
        initSubscribers(cfg, client, subscriberList->value.list);
        LOG_INFO("Client %s initialized.", id);
    }
}

static void readConfigurationFile(char* filename) {
    config_t cfg;
    config_init(&cfg);

    if(!config_read_file(&cfg, filename)){
        LOG_ERROR( 
            "Error reading config file %s\n%s:%d - %s", 
            filename, 
            config_error_file(&cfg),
            config_error_line(&cfg), 
            config_error_text(&cfg));
        config_destroy(&cfg);
        exit(EXIT_FAILURE);
    }

    config_setting_t* clientList = config_lookup(&cfg, "mqttClients");
    initClients(cfg, clientList->value.list);
}

void loop() {
    while(true){
        for(pair<mosquitto*, MqttClient*> client : clientMap){
            mosquitto_loop(client.first, -1, 1);
        }
    }
}

int main(int argc, char** argv)  {

    // Initialize log4cpp
    std::string initFileName = "log4cpp.properties";
	log4cpp::PropertyConfigurator::configure(initFileName);

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
    LOG_INFO("Mosquitto version %d.%d.%d", major, minor, revision);

    if (argc==1) {
        readConfigurationFile(config_file);
    } else {
        readConfigurationFile(argv[1]);
    }

    loop();
}
