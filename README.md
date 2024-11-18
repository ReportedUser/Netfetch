# Netfetch

A powerful command-line tool designed to provide detailed network information, perfect for managing and troubleshooting your homelab. Follow the instructions below to get started.

### Dependencies

For Debian-based systems, ensure the following packages are installed:

    $ apt get install libpsl-dev libcurl4 git build-essential

___________________________________________________________________________________________

### Installation

Clone this repository:
    
    $ git clone git@github.com:ReportedUser/Netfetch.git
    
Run the Makefile to build the application:

    $ make
    $ make install

Now you can delete the cloned repository.

### Usage

To get started, you'll need to configure the netfetch-services.conf created inside `~/.config/netfetch/` file with the necessary details for each service. Follow the structure below to add the relevant configuration:
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
- A test python service to check connectivity. -> [server-test]

You can easily create your own custom logos by following the instructions in the section below.

#### Add your custom logo!

To create a custom logo for your service, you can use the following tool:
 - [ascii-image-converter by TheZoraiz](https://github.com/TheZoraiz/ascii-image-converter)

This tool allows you to convert images into ASCII art, which can then be used as your custom logo in the configuration. The dimensions used by width and heigth are 30 and 20, respectively.

The recommended dimensions for the logo are 30 pixels in width and 20 pixels in height. To create an ASCII image with these dimensions, use the following command:
    
    $ ascii-image-converter pi-hole-logo-small.png -C -d 30,20
    
Where:

 - pi-hole-logo-small.png is the input image you want to convert.
 - -C enables color in the output.
 - -d 30,20 specifies the dimensions (width 30, height 20).

Once your ASCII image is created, follow these steps to integrate it into the project:
1. Add the Logo to logos.h: Open the include/logos.h file and add your custom logo, following the pattern of the other logos. Ensure the format is consistent with existing entries.
2. Update the search_logo Function: In the src/netfetch directory, locate the search_logo function. In this function, add your custom logo to the array of structures.

The structure will include:

 - Service Name: The name you have chosen for your service (e.g., "pi-hole").
 - Header Constant: A constant that links the logo to the correct header file.

Example:

    {"pi-hole", pihole_logo},

Replace "pi-hole" with the chosen name for the service, and pihole_logo with the header constant for the logo.

____________________________________________________________

### Comments

I'll do my best to maintain and improve this repository. If you got any ideas for new features or find any issue, please don't hesitate to tell me. :smiley:

Finally, from the time being I won't be accepting pull requests as I'm also using this project to improve my C knowledge. 
