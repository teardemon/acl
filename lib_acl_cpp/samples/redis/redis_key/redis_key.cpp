#include "stdafx.h"

static acl::string __keypre("test_key");

static bool test_del(acl::redis_key& option, int n)
{
	acl::string key;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		option.reset();
		int ret = option.del_one(key.c_str());
		if (ret < 0)
		{
			printf("del key: %s error\r\n", key.c_str());
			return false;
		}
		else if (i < 10)
			printf("del ok, key: %s\r\n", key.c_str());
	}

	return true;
}

static bool test_expire(acl::redis_key& option, int n)
{
	acl::string key;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		option.reset();
		if (option.expire(key.c_str(), 100) < 0)
		{
			printf("expire key: %s error\r\n", key.c_str());
			return false;
		}
		else if (i < 10)
			printf("expire ok, key: %s\r\n", key.c_str());
	}

	return true;
}

static bool test_ttl(acl::redis_key& option, int n)
{
	acl::string key;
	int ttl;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		option.reset();
		if ((ttl = option.ttl(key.c_str())) < 0)
		{
			printf("get ttl key: %s error\r\n", key.c_str());
			return false;
		}
		else if (i < 10)
			printf("ttl ok, key: %s, ttl: %d\r\n",
				key.c_str(), ttl);
	}

	return true;
}

static bool test_exists(acl::redis_key& option, int n)
{
	acl::string key;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		option.reset();
		if (option.exists(key.c_str()) == false)
			printf("no exists key: %s\r\n", key.c_str());
		else
			printf("exists key: %s\r\n", key.c_str());
	}

	return true;
}

static bool test_type(acl::redis_key& option, int n)
{
	acl::string key;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		option.reset();
		acl::redis_key_t ret = option.type(key.c_str());
		if (ret == acl::REDIS_KEY_UNKNOWN)
		{
			printf("unknown type key: %s\r\n", key.c_str());
			return false;
		}
		else
			printf("type ok, key: %s, ret: %d\r\n",
				key.c_str(), ret);
	}

	return true;
}

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"-s redis_addr[127.0.0.1:6379]\r\n"
		"-n count\r\n"
		"-C connect_timeout[default: 10]\r\n"
		"-T rw_timeout[default: 10]\r\n"
		"-a cmd[del|expire|ttl|exists|type|all]\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	int  ch, n = 1, conn_timeout = 10, rw_timeout = 10;
	acl::string addr("127.0.0.1:6379"), cmd;

	while ((ch = getopt(argc, argv, "hs:n:C:T:a:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addr = optarg;
			break;
		case 'n':
			n = atoi(optarg);
			break;
		case 'C':
			conn_timeout = atoi(optarg);
			break;
		case 'T':
			rw_timeout = atoi(optarg);
			break;
		case 'a':
			cmd = optarg;
			break;
		default:
			break;
		}
	}

	acl::acl_cpp_init();
	acl::redis_client client(addr.c_str(), conn_timeout, rw_timeout);
	acl::redis_key option(&client);

	bool ret;

	if (cmd == "del")
		ret = test_del(option, n);
	else if (cmd == "expire")
		ret = test_expire(option, n);
	else if (cmd == "ttl")
		ret = test_ttl(option, n);
	else if (cmd == "exists")
		ret = test_exists(option, n);
	else if (cmd == "type")
		ret = test_type(option, n);
	else if (cmd == "all")
	{
		ret = test_expire(option, n)
			&& test_ttl(option, n)
			&& test_exists(option, n)
			&& test_type(option, n)
			&& test_del(option, n);
	}
	else
	{
		ret = false;
		printf("unknown cmd: %s\r\n", cmd.c_str());
	}

	if (ret == true)
		printf("test OK!\r\n");
	else
		printf("test failed!\r\n");

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif
	return 0;
}
