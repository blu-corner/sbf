#!/usr/bin/python3
import configparser
import argparse
import socket


class ConfigFile(dict):

    def __init__(self):
        dict.__init__(self)

    @staticmethod
    def load(path):
        instance = ConfigFile()
        instance.loadFromFile(path)
        return instance

    def loadFromFile(self, path):
        print("loading config file:", path)
        with open(path, 'r') as fd:
            parser = configparser.ConfigParser()
            parser.readfp(fd)

            for section in parser.sections():
                self[section] = {}
                items = parser.items(section)

                for name, value in items:
                    self[section][name] = value


class ServerConfig(ConfigFile):

    @property
    def isUnix(self):
        return self['server']['type'] == 'unix'

    @property
    def isInet(self):
        return self['server']['type'] == 'inet'

    @property
    def unixPath(self):
        return self['server']['path']

    @property
    def inetHost(self):
        return self['server']['host']

    @property
    def inetPort(self):
        return self['server']['port']

    @staticmethod
    def load(path):
        instance = ServerConfig()
        instance.loadFromFile(path)
        return instance


class Client:

    def __init__(self, config):
        self._config = ServerConfig.load(config)
        self._socket = None

        try:
            if self._config.isUnix:
                self._socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            elif self._config.isInet:
                self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            else:
                raise Exception("Unknown server type")
        finally:
            Client.connect(self)

    @property
    def config(self):
        return self._config

    @property
    def socket(self):
        return self._socket

    def connect(self):
        self.socket.connect(self.serverAddr)
        print('Connected\nCtrl-C to quit\n"DONE" to shut down server')

        while True:
            try:
                data = input("> ")
                if "" != data:
                    print("Sent:", data)
                    self.socket.send(data.encode('utf-8'))
                    if "DONE" == data:
                        print("Closing")
                        break
            except KeyboardInterrupt:
                print("Shutting down")
                self.socket.close()
                break

    @property
    def serverAddr(self):
        if self.config.isUnix:
            return self.config.unixPath
        return (self.config.inetHost, int(self.config.inetPort))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Config file for server")
    parser.add_argument("--c", help="Config file for server declaration")
    args = parser.parse_args()
    if args.c is None:
        print("Please provide config file")
    else:
        Client(args.c)
