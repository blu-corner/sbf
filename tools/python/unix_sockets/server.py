#!/usr/bin/python3
import configparser
import argparse
import signal
import socket
import time
import os


class ConfigFile(dict):

    def __init__(self):
        dict.__init__(self)

    @staticmethod
    def load(path):
        instance = ConfigFile()
        instance.loadFromFile(path)
        return instance

    def loadFromFile(self, path):
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


class Server:

    def __init__(self, config):
        self._config = ServerConfig.load(config)
        self._socket = None
        signal.signal(signal.SIGUSR1, self.recieve_signal)

        try:
            if self._config.isUnix:
                self._socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            elif self._config.isInet:
                self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            else:
                raise Exception("Unknown server type")
        finally:
            Server.dispatch(self)

    @property
    def config(self):
        return self._config

    @property
    def socket(self):
        return self._socket

    def dispatch(self):
        if self.config.isUnix and os.path.exists(self.serverAddr):
            os.remove(self.serverAddr)

        self.socket.bind(self.serverAddr)
        self.socket.listen(5)

        print('Socket open\nPID = {0}'.format(os.getpid()))

        conn, addr = self._socket.accept()

        while True:
            data = conn.recv(1024)
            if not data:
                break
            else:
                print(data.decode('utf-8'))

    def recieve_signal(self, signum, stack):
        self.socket.close()
        time.sleep(3)
        Server(args.c)

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
        Server(args.c)
