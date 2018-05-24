#include <SPI.h>
#include <Ethernet.h>
#include <avr/wdt.h>

const char user_0[] PROGMEM = 	"dXNlcjE6cGFzczE="; 				// user1:pass1
const char user_1[] PROGMEM = 	"dXNlcjI6cGFzczI=";					// user2:pass2
const char user_2[] PROGMEM = 	"dXNlcjM6cGFzczM=";					// user3:pass3
const char user_3[] PROGMEM = 	"cGF0cmljazpzZW5oYQ==";				// patrick:senha
const char user_4[] PROGMEM = 	"OGRpZ2l0b3M6bWFpczhkaWc=";			// 8digitos:mais8dig
const char user_5[] PROGMEM = 	"String 5";
const char user_6[] PROGMEM = 	"String 6";
const char user_7[] PROGMEM = 	"String 7";
const char user_8[] PROGMEM = 	"String 8";
const char user_9[] PROGMEM = 	"String 9";
const char user_10[] PROGMEM = 	"String 10";

int ultimologin[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

const char* const user_table[] PROGMEM = {	user_0,	user_1, user_2, user_3, user_4, user_5, 
											user_6, user_7, user_8, user_9, user_10			};
											
char buffer[35];    // o tamanho deve ser ser maior que "maior string de usuario + 1"
// strcpy_P(buffer, (char*)pgm_read_word(&(user_table[i])));     USAR PARA TRANSFERIR UMA DAS STRINGS AO BUFFER NA RAM


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1, 111);

bool authentificated = false;
String header;
unsigned long int millisLogin = 0;
unsigned long int millisLogin_old = 0;
unsigned long int millisSimultaneo = 0;

bool teste = 0;

bool rele1 = 0;
const int bt = 2; 
const int contato1 = 7;


// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() 
{
	wdt_disable();
	wdt_enable(WDTO_8S);
	// Open serial communications and wait for port to open:
	Serial.begin(9600);

	
	// start the Ethernet connection and the server:
	Ethernet.begin(mac, ip);
	server.begin();
}


void SendOKpage(EthernetClient &client)
{
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	client.println("Connnection: close");
	client.println();
	client.println("<!DOCTYPE HTML>");
	client.println("<html>");
	// Refresh tag: pull a cada 4 segundos
	client.println("<meta http-equiv=\"refresh\" content=\"4\">");
	
	// conteudo do cliente (Paulo)
	client.println("<form action='http://192.168.1.111/1d' method=''>");
	client.println("<BR><H1>RESET DA REMOTA SE BER II </H1><BR><BR>SELECIONE O COMANDO DE RESET DA REMOTA:<BR>");
	
	client.println("<BR><input type='submit'value='DESLIGA'> <br><br> </form>");
	
	client.println("<form action='http://192.168.1.111/1l' method=''>");
	client.println("<BR><input type='submit'value='LIGA'> <br><br> </form>");
	
	if(rele1==1)
	{  
		digitalWrite(contato1, HIGH);
		client.println("rele1 esta ligado<br>");
	} 
	else if (rele1==0)
	{
		digitalWrite(contato1, LOW);
		client.println("rele1 esta desligado<br>");
	}
	
	if (digitalRead(bt) == HIGH)
	{     
		client.println("UTR LIGADA<br><br>"); 
	}
	else
	{
		client.println("UTR DESLIGADA<br><br>");  
	}
	
	client.println("<H2>HISTORICO DE ACESSOS</H2><BR>");
	for (int i = 0; i<10; i++)
	{
		client.println(i+1);
		client.print(": ");
		if (ultimologin[i] == -1)	client.print("<br>");
		
		// mudar para os nome de usuario desejados, cadastrados em user_0 - user_10.
		if (ultimologin[i] == 0)	client.print("USUARIO 0<br>");		
		if (ultimologin[i] == 1)	client.print("USUARIO 1<br>");
		if (ultimologin[i] == 2)	client.print("USUARIO 2<br>");		
		if (ultimologin[i] == 3)	client.print("USUARIO 3<br>");
		if (ultimologin[i] == 4)	client.print("USUARIO 4<br>");		
		if (ultimologin[i] == 5)	client.print("USUARIO 5<br>");
		if (ultimologin[i] == 6)	client.print("USUARIO 6<br>");		
		if (ultimologin[i] == 7)	client.print("USUARIO 7<br>");
		if (ultimologin[i] == 8)	client.print("USUARIO 8<br>");		
		if (ultimologin[i] == 9)	client.print("USUARIO 9<br>");
		if (ultimologin[i] == 10)	client.print("USUARIO 10<br>");
	}

	client.print("<br><br>usuarios simultaneos: ");
	if (teste == 0)	client.print("NAO<br><br>");
	else if (teste == 1) client.print("SIM<br><br>");
	
	client.println(millisLogin - millisSimultaneo);
	
	client.println("</html>");	
}

void SendAuthentificationpage(EthernetClient &client)
{
	client.println("HTTP/1.1 401 Authorization Required");
	client.println("WWW-Authenticate: Basic realm=\"Secure Area\"");
	client.println("Content-Type: text/html");
	client.println("Connnection: close");
	client.println();
	client.println("<!DOCTYPE HTML>");
	client.println("<HTML>  <HEAD>   <TITLE>Error</TITLE>");
	client.println(" </HEAD> <BODY><H1>401 Unauthorized.</H1></BODY> </HTML>");
}


void loop() 
{
	// listen for incoming clients
	EthernetClient client = server.available();
	if (client) 
	{
		Serial.print("new client: ");
		Serial.println(client);
		authentificated = false;
		// an http request ends with a blank line
		boolean currentLineIsBlank = true;
		while (client.connected()) 
		{
			if (client.available()) 
			{
				char c = client.read();
				header += c;
				
				if (header.endsWith("/1l") && authentificated)
				{
					rele1 = 1;
				}
				else if (header.endsWith("/1d") && authentificated)
				{
					rele1 = 0;
				}
				
				// if you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply
				if (c == '\n' && currentLineIsBlank) 
				{
					if (authentificated)
					{
						SendOKpage(client); 
					}
					else
					{
						SendAuthentificationpage(client);
					}
					break;
				}
				
				if (c == '\n') 
				{
					// you're starting a new line
					currentLineIsBlank = true;
					if (header.indexOf("Authorization: Basic") >=0)
					{
						for (int i=0; i < 11; i++)
						{
							if (authentificated != true)
							{
								// transfere string de usuario i para um buffer na RAM
								strcpy_P(buffer, (char*)pgm_read_word(&(user_table[i]))); 
								
								if (header.indexOf(buffer) >=0)
								{
									authentificated = true;
									millisLogin_old = millisLogin;
									millisLogin = millis();
								
									if (millisLogin - millisLogin_old < 3950)
									// se ha mais de um acesso simultaneo
									{
										millisSimultaneo = millisLogin;
										teste = 1;
										if (ultimologin[0] != i)
										{
											if (ultimologin[1] != i)
											
											{
												for (int j=9; j >= 1; j--)
												{
													ultimologin[j] = ultimologin[j-1];
												}
												ultimologin[0] = i;
											}
										}
									}
									
									else if (ultimologin[0] != i)
									{
										if (ultimologin[1] != i  || millisLogin - millisSimultaneo > 20000)
										//				   || ja se passou mais de 2min desde o ultimo login simult. (120000)
										{
											for (int j=9; j >= 1; j--)
											{
												ultimologin[j] = ultimologin[j-1];
											}
											ultimologin[0] = i;
										}
									}
								}
							}
						}
					}
					header = "";		
				} 
				else if (c != '\r') 
				{
					// you've gotten a character on the current line
					currentLineIsBlank = false;
					wdt_reset();
				}
			}
		}
		// give the web browser time to receive the data
		delay(1);
		// close the connection:
		client.stop();
		teste = 0;
	}
}