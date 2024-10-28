# Netfetch
A command-line network information tool for your homelab. Read this instruction to learn how it's used.

#### Dependencies.

For Debian make sure you got:

    $ apt get install libpsl-dev libcurl4

#### Installation.

Clone this repository and launch the Makefile:
    
    $ git clone git@github.com:ReportedUser/Netfetch.git
    $ ./Makefile

#### Usage.

In order to use this cli application, it's necessary to add some information to the config.txt file. Follow this structure:

```
[service-name]
link=linkwheretheAPIis
value_1= ...
value_2= ...
...
value_6= ...
```

Where value_X=... is the key value. Here is an example using the Pi-hole API service:

```
[pi-hole] #This is the name used to find the logo.
link=http://198.186.10.34/admin/api.php?summaryRaw&auth=34jhdasfl435adfs2a324sdaf  #The API key is added to the link, this will be changed in the future.
value_1=status
value_2=dns_queries_today
value_3=ads_blocked_today
value_4=ads_percentage_today
```

There are a lot of posible services, I'm don't make use of a lot of them so this list will never be completly filled.
If there is a service that you feel like it's really used and believe it should be added, say so and it will be added to the default list.


These are the current default logos:

- Pi-hole -> [pi-hole]

You can create your own logo with the instructions on this next section.

#### Create your own service logo!

The tool used for the cli art is the following: [ascii-image-converter by TheZoraiz](https://github.com/TheZoraiz/ascii-image-converter)

Characteristics used are the following:

NEED TO COMPLETE
