# Protocolo 1.3

## C0123 Grupo 3

### Introducción

Este protocolo define la comunicación entre los servidores, tenedores y clientes de un conjunto de seis islas. Cada isla tiene un servidor, un cliente y un tenedor que funcionan de forma independiente, pero siguen un esquema común de comunicación. El objetivo del protocolo es permitir la solicitud y transferencia de libros entre las islas utilizando un sistema basado en broadcast para anunciar los libros disponibles, y una transferencia de datos segmentada en bloques de 256 bytes. El protocolo utiliza HTTP como base de la comunicación.

### Elementos del Sistema

- **Servidor:** Posee un sistema de archivos elegido por cada equipo independientemente, donde almacena uno o más libros. Se encarga de enviar un broadcast anunciando los libros disponibles, además de recibir solicitudes de parte de los tenedores y enviar los que sean solicitados en bloques de 256 bytes. Igualmente escucha los mensajes de broadcast de los Tenedores solicitando la lista de libros.

- **Tenedor:** Escucha los mensajes de broadcast de otros servidores y recopila la información sobre los libros disponibles en otras islas. No tiene la posibilidad de ver a los demás tenedores, se limita únicamente a comunicarse con los servidores disponibles. Su función es servir de intermediario entre el cliente y los servidores.

- **Cliente:** Solicita al tenedor de su isla la lista de libros disponibles en las demás islas a través de la
solicitud GET de protocolo HTTP enviado a través de la URL. Una vez seleccionado un libro, el tenedor se comunica con el servidor correspondiente para descargar el libro en bloques de 256 bytes y enviarlo al cliente.

### Proceso de Broadcast

1. Cuando un servidor se levanta:
    - El servidor tiene un libro disponible en su sistema de archivos.
    - Inmediatamente envía un **broadcast** en la red para anunciar este libro.
    - Este mensaje de broadcast contiene la **IP** del servidor y el **nombre del libro**.
    - Se queda escuchando por si algún tenedor se levanta tarde y necesita la lista de libros.

2. Cuando el tenedor se levanta:
    - El tenedor se pone a la escucha de las direcciones de **broadcast** predefinidas.
    - Si algún servidor en la red envía un mensaje de broadcast, el tenedor lo recibe y lo almacena.
    - El tenedor reúne los datos de todos los servidores activos en otras islas, creando una lista de libros disponibles.
    - El servidor al levantarse envía broadcast a todos los servidores para que estos envíen sus listas de libros.
    - Se queda escuchando por si algún servidor se levanta y envía la lista de libros.

### Formato del Mensaje de Broadcast

El servidor envía un mensaje de formato simple, utilizando un protocolo específico (UDP) en una dirección de broadcast para toda la red.

    ```http
    Broadcast enviado desde Servidor
    --------------------------------
    Libro: Aventuras-en-la-Isla
    IP del Servidor: 172.16.123.36
    Puerto: 8080
    ```

Formato de petición.
  - Método: “GET”
  - URI: “/libros” // “/nombrelibro”
  - Versión HTTP: “HTTP/1.0”
  - Cuerpo: Ignorado

Formato de respuesta.
  - Estado: “OK” 200
  - Versión HTTP: “HTTP/1.0”
  - Cuerpo: Listado de libros o libro específico

### Flujo Completo del Broadcast

1. El servidor envía un broadcast:
    - Envía un mensaje de tipo **UDP** a una dirección de broadcast como `172.16.123.31:8000`
    - El mensaje contiene el nombre del libro, la IP y el puerto donde puede ser solicitado.

2. El tenedor escucha el broadcast:
    - El tenedor en cada isla está escuchando constantemente en el puerto de broadcast (8000).

3. Cliente solicita la lista de libros:
    - Cuando el cliente solicita los libros disponibles, el tenedor le envía la lista con los nombres de los libros y la IP de los servidores.

Con esta estructura, los tenedores de cada isla podrán recopilar información sobre los libros disponibles en otras islas y presentarla a los clientes.

### Direcciones IP y Rango de Broadcast

Cada isla tiene una dirección IP definida de la forma `172.16.123.X`, donde `X` varía dependiendo de la computadora donde se levante el servidor en la isla. El broadcast se realiza en direcciones predefinidas para cada isla:

- **Isla 1:**  
  IP: `172.16.123.X`  
  Broadcast: `172.16.123.31`

- **Isla 2:**  
  IP: `172.16.123.X`  
  Broadcast: `172.16.123.47`

- **Isla 3:**  
  IP: `172.16.123.X`  
  Broadcast: `172.16.123.63`

- **Isla 4:**  
  IP: `172.16.123.X`  
  Broadcast: `172.16.123.79`

- **Isla 5:**  
  IP: `172.16.123.X`  
  Broadcast: `172.16.123.95`

- **Isla 6:**  
  IP: `172.16.123.X`  
  Broadcast: `172.16.123.111`

### Flujo de Comunicación

1. **Inicio del Servidor:**  
   Cuando un servidor se levanta, envía un mensaje de broadcast en su red con la información del libro disponible, que incluye su **IP** y el **nombre del libro**. Este mensaje estará esperando a ser capturado por un tenedor. Luego el servidor se quedará escuchando por si algún tenedor se levanta tarde y necesita la lista de libros.

2. **Inicio del Tenedor:**  
   Si el tenedor se inicia antes de que algún servidor esté disponible, este enviará broadcast a las demás islas para que le envien la lista de libros y luego permanecerá a la escucha en los puertos de broadcast definidos, recopilando la información de los servidores activos en otras islas. El tenedor compila una lista de libros disponibles en las diferentes islas a partir de los mensajes de broadcast. Si el Tenedor se levanta despúes de los servidores, este debe enviar un broadcast avisando que se levantó para que los Servidores vuelvan a enviar las listas de libro, este broadcast lleva un mensaje simple de "Libros".

   ```http
    Broadcast enviado desde Tenedor
    --------------------------------
    Libros
    ```

3. **Solicitudes del Cliente:**  
   El cliente solicita al tenedor la lista de libros disponibles en las islas. Tras recibir la lista, el cliente selecciona el libro deseado, y el tenedor efectúa la comunicación con el respectivo servidor, dependiendo del libro que se haya solicitado y finalmente lo devuelve al cliente.

4. **Transferencia del Libro:**  
   El cliente se comunica con el tenedor, y este luego con el servidor para solicitar el libro seleccionado. El servidor transfiere el libro en bloques de **256 bytes**, de acuerdo con las capacidades de su sistema de archivos. La comunicación sigue el protocolo HTTP.

5. **Persistencia del Sistema:**  
   Tanto los servidores como los tenedores permanecen a la escucha de nuevos mensajes hasta que otro componente del sistema se active o se realice una nueva solicitud.

### Ejemplo Completo del Proceso

1. **Cliente solicita libros disponibles**:c
   ```http
   GET /libros HTTP/1.1
   Host: tenedor-isla
    ```

2. **Respuesta del Tenedor**:
    ```http
   HTTP/1.1 200 OK
    Content-Type: application/...
    Content-Length: 123

    Cocorí
    Aventuras en la Isla
    .
    .
    .
    ```

3. **Cliente solicita un libro específico**:
    ```http
   GET /Aventuras-en-la-Isla HTTP/1.1
   Host: tenedor-isla
    ```

4. **El Tenedor solicita al servidor**:
    ```http
   GET /Aventuras-en-la-Isla HTTP/1.1
   Host: 172.16.123.34
    ```

5. **Servidor responde al Tenedor**:
    ```http
    HTTP/1.1 200 OK
    Content-Type: application/...
    Content-Length: 2048

    <primeros_256_bytes_de_datos>
    <segundos_256_bytes_de_datos>
    ...
    ```

6. **Tenedor responde al Cliente**:
    ```http
    HTTP/1.1 200 OK
    Content-Type: application/...
    Content-Length: 2048

    <primeros_256_bytes_de_datos>
    <segundos_256_bytes_de_datos>
    ...
    ```

### Puertos Utilizados

- **Puerto de Broadcast (definido):**  
  Todos los mensajes de broadcast entre servidores y tenedores utilizan un puerto dedicado para evitar interferencias con otras comunicaciones. Este puerto es el **8000**.

- **Puerto de Transferencia de Libros:**  
  El intercambio de datos entre el cliente y el servidor se realiza en un puerto separado, el **8080**, exclusivo para la transferencia de libros.

### Consideraciones Técnicas

- **Concurrencia con Hilos:**  
  Tanto los servidores como los tenedores deben implementar hilos que permanezcan escuchando los mensajes de broadcast de manera concurrente, sin bloquear otras operaciones del sistema.

- **Bloques de 256 Bytes:**  
  La transferencia de libros se realiza en bloques de hasta **256 bytes** para garantizar una transferencia segura y manejable, independientemente del sistema de archivos usado en cada servidor.

- **Aislamiento de Tenedores:**  
  Los tenedores no se comunican entre sí, manteniendo una separación clara entre las islas. Solo interactúan con los servidores de su propia isla y los de otras islas mediante broadcast.

- **Aislamiento de Clientes:**  
  Los clientes solo pueden comunicarse con el tenedor de su respectiva isla, no pueden ver a ningún otro servidor, cliente o tenedor de otra isla.

### Casos a Contemplar 

- **Error en la red del Tenedor y Servidor:**
  El tenedor no puede conectar con ningún servidor debido a una caída de la red. Este devuelve un estado 503 Service Unavailable para indicar que el servicio no está disponible temporalmente.

- **Timeout en la respuesta del Servidor:**
  El Servidor tarda demasiado en responder a la solicitud del Tenedor, así que devuelve un estado 504 Gateway Timeout, indicando que no llegó a tiempo.

- **Fallo en el Servidor:**
  El servidor no puede responder a solicitudes del Tenedor por errores internos y devuelve un estado 500 Internal Server Error para indicar que ocurrió un error interno.

- **Solicitud malformada del Cliente:**
  El Cliente envía una solicitud malformada(ejemplo: Get /) o utiliza un método no soportado, así que devuelve un estado 400 Bad Request o 405 Method Not Allowed para informar al cliente sobre el error.

- **Colisión de puertos:**
  Dos Tenedores intentan utilizar el mismo puerto, provocando conflictos. El sistema debe garantizar que los puertos están correctamente definidos para cada comunicación entre Tenedores y otros componentes.

- **Sobrecarga del Tenedor:**
  El Tenedor recibe un número excesivo de solicitudes simultánemente y su capacidad se ve sobrepasada, así que es necesario implementar un sistema de colas o limitación de tasa para evitar esto, devolviendo un 429 Too Many Requests si es necesario.

- **Levantamiento de Tenedor:**
  Si un Tenedor se levanta después de un Servidor, este no va a poder obtener la lista de libros de ese Servidor, por lo tanto, este va a enviar una señal de broadcast a todas las islas para que los Servidores que estén levantados, envíen la lista de libros.

- **Tenedores y Servidores esuchando:**
  Los Servidores y Tenedores deben tener un hilo o proceso que se encargue de permanecer a la escucha de nuevas señales de broadcast, para así mantener la lista de libros actualizada y estar disponibles para cualquiera que se levante.

### Cierre

Este protocolo garantiza una comunicación eficiente y organizada entre las islas, facilitando la transferencia de libros entre los servidores y los clientes a través de los tenedores, manteniendo una arquitectura descentralizada y basada en HTTP.

**Versión 1.3**