## Monitor Infrastructure - Tests Examples

&copy; Atos 2016

C program that connects with Collectd. 

#### Requirements
##### */etc/collectd/collectd.conf*

1. This example requires **Collectd** to be configured with the **unixsock**, **rrdtool** and **csv** plugins:

	```
	<Plugin unixsock>
		SocketFile "/var/run/collectd-unixsock"
		SocketGroup "collectd"
		SocketPerms "0775"
		DeleteSocket false
	</Plugin>
	```
2. Create custom types
3. Uncomment *TypesDB* line in */etc/collectd/collectd.conf* and add path to custom types
4. Restart collectd

	```
	sudo service collectd restart
	```

#### Run program

1. Compile

	> gcc example.c -o example
2. Run program