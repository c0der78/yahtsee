#include <arg3net/http_client.h>
#include <arg3json/json.h>
#include <arg3net/uri.h>
#include <map>
#include <cstring>

using namespace std;
using namespace arg3;
using namespace arg3::net;

void show_help(const char *program);

const char *const supported_formats[] = { "form", "json" };

bool is_supported_format(const string &format);

bool build_data(map<string, string> &data, char *arg);

string data2form(const map<string, string> &data);
string data2json(const map<string, string> &data);

#ifndef DEBUG
#define GAME_API_URL "connect.arg3.com"
#define APP_ID "51efcb5839a64a928a86ba8f2827b31d"
#define APP_TOKEN "78ed4bfb42f54c9fa7ac62873d37228e"
#else
#define GAME_API_URL "localhost.arg3.com:1337"
#define APP_ID "8846b98d082d440b8d6024d723d7bc24"
#define APP_TOKEN "ac8afc408f284eedad323e1ddd5c17e4"
#endif

int main(int argc, char *argv[])
{
    int c;

    string host, path, format = "json";

    map<string, string> data;

    http::method method = http::POST;

    while ((c = getopt(argc, argv, "h:m:f:d:")) != -1)
    {
        switch (c)
        {
        case 'h':
            host = optarg;
            break;
        case 'm':
            for (int i = 0; i < sizeof(http::method_names) / sizeof(http::method_names[0]); i++)
            {
                if (!strcasecmp(http::method_names[i], optarg))
                {
                    method = static_cast<http::method>(i);
                    break;
                }
            }
            break;
        case 'f':
            format = optarg;
            if (!is_supported_format(format))
            {
                printf("%s is an unsupported format.\n", format.c_str());
                return 1;
            }
            break;
        case 'd':
            if (!build_data(data, optarg))
                return 1;
            break;
        }

    }

    if (argc <= optind || !argv[optind] || !*argv[optind])
    {
        show_help(argv[0]);
        return 1;
    }

    if (host.empty())
    {
        arg3::net::uri uri(argv[optind]);

        auto port = uri.port();

        if (port.empty())
            host = uri.host();
        else
            host = uri.host() + ":" + port;

        path = uri.path();

        printf("connecting to %s%s\n", host.c_str(), path.c_str());
    }
    else
    {
        path = argv[optind];
    }

    http_client client(host);

    if (!data.empty())
    {
        if (!strcasecmp(format.c_str(), "json"))
        {
            client.add_header("Content-Type", "application/json");
            client.set_payload(data2json(data));
        }
        else
        {
            client.add_header("Content-Type", "application/x-www-form-urlencoded");
            client.set_payload(data2form(data));
        }
    }
    client.request(method, path);

    printf("Response: %d\n", client.response_code());
    puts(client.response().c_str());

    return client.response_code() == http::OK ? 0 : 1;
}

void show_help(const char *program)
{
    printf("Syntax: %s (options) <url>\n", program);
    printf("      : %s (options) -h <host> <path>\n", program);
    printf("Options:\n");
    printf(" -m : http method (GET,POST,PUT,DELETE) default is POST\n");
    printf(" -f : format of input data (json,form) default is form\n");
    printf(" -d : data (command separated key values, key1=v1,key2=v2)\n");
}

bool is_supported_format(const string &format)
{
    bool found = false;

    for (int i = 0; i < sizeof(supported_formats) / sizeof(supported_formats[0]); i++)
    {
        if (!strcasecmp(supported_formats[i], format.c_str()))
        {
            found = true;
            break;
        }
    }

    return found;
}

bool build_data(map<string, string> &data, char *arg)
{
    char *p = strtok(arg, ",");

    while (p != NULL)
    {
        char *value;
        char *key = strtok_r(p, "=,", &value);

        if (key && value)
        {
            data[key] = value;
        }
        else
        {
            printf("Invalid data argument: %s", p);
            return false;
        }
        p = strtok(NULL, ",");
    }

    return true;
}

string data2json(const map<string, string> &data)
{
    json::object obj;

    for (auto & entry : data)
    {
        obj.set_string(entry.first, entry.second);
    }

    return obj.to_string();
}

string data2form(const map<string, string> &data)
{
    ostringstream buf;

    int count = 0;
    for (auto & entry : data)
    {
        buf << entry.first << "=" << entry.second;

        if (++count != data.size())
        {
            buf << "&";
        }
    }
    return buf.str();
}