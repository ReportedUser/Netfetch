//logos.h

#ifndef ASCII_LOGOS_H
#define ASCII_LOGOS_H
#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define RESET "\033[0m"


const char* show_general = 
"%s %s %s %s %s \n";


const char* pihole_logo =
GREEN " :+++=-:.			\n"	
GREEN "  ++++++++-.			\t" RESET"\033[1m [%s]	\n"RESET
GREEN "  :+++===+++-     .:-===		" RESET" %s	\n"
GREEN "   :++++===++=  .=+++++-		" RESET" %s 	\n"
GREEN "     :=+++=--+:.+++++=:		\t" RESET" %s 	\n"
GREEN "       .:-==. -=+=-:.		\t" RESET" %s 	\n"
RED "            :.:::.			\n"
RED "         .:--------:.		\n"
RED "       .:------------:.		\n"
RED "     .:--------------:::.		\n"
RED "   .:::::-::-------:::::::.	\n"
RED " .:::::::::. .......::::::::.	\n"
RED "::::::::::::      .:::::::::::	\n"
RED "::::::::::::      ::::::::::::	\n"
RED " .::::::::. ..... .::::::::::	\n"
RED "   .::::::::------:::::::::.	\n"
RED "     .:::---------------:.		\n"
RED "       .:-------------:		\n"
RED "         .:--------:.		\n"
RED "           .::::::.		\n";



const char* test_logo =
"YES, THIS IS A BIIIIIG LOGO.\t [%s]\n"
"YES, THIS IS A BIIIIIG LOGO.\t %s\n"
"YES, THIS IS A BIIIIIG LOGO.\t %s\n"
"YES, THIS IS A BIIIIIG LOGO.\t %s\n"
"YES, THIS IS A BIIIIIG LOGO.\t %s\n"
"YES, THIS IS A BIIIIIG LOGO.\t %s\n";

#endif // ASCII_LOGOS_H
