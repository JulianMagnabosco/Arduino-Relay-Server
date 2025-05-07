#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //Direccion Fisica MAC
byte ip[] = { 192, 168, 2, 240 };                       // IP Local que usted debe configurar
byte gateway[] = { 192, 168, 2, 2 };                   // Puerta de enlace
byte subnet[] = { 255, 255, 255, 0 };                  //Mascara de Sub Red
EthernetServer server(80);                             //Se usa el puerto 80 del servidor
String HTTP_req;

bool pins[] = {0, 0, 0, 0, 0, 0, 0, 0};
int sizeVar = 8;
void setup() {
//  delay(200);

  Serial.begin(9600);  // Inicializa el puerto serial
  while (!Serial) {   // Espera a que el puerto serial sea conectado, Solo necesario para el Leonardo
    ;
  }
  for (int i = 0; i < sizeVar; i++) {
    pinMode(i + 2, OUTPUT);     // Se configura como salidas los puertos del 2 al 9
  }
  Ethernet.begin(mac); // Inicializa la conexion Ethernet y el servidor
  Ethernet.setGatewayIP(gateway);
  Ethernet.setSubnetMask(subnet);
  server.begin();
  Serial.print("El Servidor es: ");
  Serial.println(Ethernet.localIP());    // Imprime la direccion IP Local
}


void loop() {
  // Crea una conexion Cliente
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //Lee caracter por caracter HTTP
        if (HTTP_req.length() < 100) {
          //Almacena los caracteres a un String
          HTTP_req += c;

        }

        // si el requerimiento HTTP fue finalizado
        if (c == '\n') {
          Serial.println(HTTP_req); //Imprime en el monitor serial

          client.println("HTTP/1.1 200 OK");           //envia una nueva pagina en codigo HTML
          client.println("Content-Type: text/html");
          client.println();
          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<TITLE>Ethernet Arduino</TITLE>");
          client.println("</HEAD>");
          client.println("<BODY>");
          client.println("<hr />");
          client.println("<H1>Arduino Ethernet Relay - Control para servidores</H1>");
          client.println("<p style=\"font-size:30px\">");

          Check();
          for (int i = 0; i < sizeVar; i++) {
            client.println(String(pins[i]) + "---");
            client.println("<input name=\"On"+ String(i) );
            client.print("\" style=\"background:red; font-size:30px ; color:white\" onclick=\"location.href='/?button" +  String(i));
            client.print("on'\" type=\"button\" value=\"On\" />");           
            client.println(" | | | | | ");
            client.println("<input name=\"Off"+  String(i) );
            client.print("\" style=\"background:green; font-size:30px; color:white\" onclick=\"location.href='/?button" + String(i));
            client.print("off'\" type=\"button\" value=\"Off\" />");
            client.println("<br />");
            client.println("<br>");
          }
          client.println("<p>");
          client.println("</BODY>");
          client.println("</HTML>");

          delay(1);
          //detiene el cliente servidor
          client.stop();

          // Limpia el String(Cadena de Caracteres para una nueva lectura
          HTTP_req = "";

        }
      }
    }
  }
}

//control del arduino si un boton es presionado
void Check() {
  for (int i = 0; i < sizeVar; i++) {
    String s = "?button" + String(i);
    if (HTTP_req.indexOf(s+"on") > -1) {
      pins[i] = 1;
    }
    if (HTTP_req.indexOf(s+"off") > -1) {
      pins[i] = 0;
    }
    digitalWrite(i+2, pins[i]);
  }
}
