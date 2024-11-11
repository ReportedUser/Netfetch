# Netfetch

A powerful command-line tool designed to provide detailed network information, perfect for managing and troubleshooting your homelab. Follow the instructions below to get started.

#### Dependencies.

For Debian-based systems, ensure the following packages are installed:

    $ apt get install libpsl-dev libcurl4 git build-essential

#### Installation.

Clone this repository:
    
    $ git clone git@github.com:ReportedUser/Netfetch.git
    
Run the Makefile to build the application:

    $ make

#### Usage.

To get started, you'll need to configure the config.txt file with the necessary details for each service. Follow the structure below to add the relevant configuration:
```
[service-name]
link=linkwheretheAPIis
value_1= ...
value_2= ...
...
value_6= ...
```

Where value_X=... represents key-value pairs. Here’s an example configuration for the Pi-hole API service:

```
[pi-hole] #This is the name used to find the logo.
link=http://198.186.10.34/admin/api.php?summaryRaw&auth=34jhdasfl435adfs2a324sdaf  #The API key is added to the link, this will be changed in the future.
value_1=status
value_2=dns_queries_today
value_3=ads_blocked_today
value_4=ads_percentage_today
```

Please note, while many services are supported, I may not use all of them, so this list might not be fully comprehensive. If there’s a service you think should be included by default, feel free to suggest it, and I’ll add it to the list.


These are the current default logos:

- Pi-hole -> [pi-hole]

You can easily create your own custom logos by following the instructions in the section below.

#### Create your own service logo!

To create a custom logo for your service, you can use the following tool:
 - [ascii-image-converter by TheZoraiz](https://github.com/TheZoraiz/ascii-image-converter)

This tool allows you to convert images into ASCII art, which can then be used as your custom logo in the configuration.


Characteristics used are the following:

NEED TO COMPLETE
