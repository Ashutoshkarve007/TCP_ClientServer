TCP Client–Server Application
=============================

1. Unzip “TCP_ClientServer.zip” anywhere on your Windows PC.

2. Open TWO separate Command Prompt windows:

   Window 1 (Server):
   ------------------
   C:\> cd \path\to\unzipped\folder
   C:\path\to\unzipped\folder> server.exe
   [Server starts and listens…]

   Window 2 (Client):
   ------------------
   C:\> cd \path\to\unzipped\folder
   C:\path\to\unzipped\folder> client.exe
   [Client connects to the server…]

3. In the client window, enter commands:
     SELECT OPEN 5
     EXECUTE
     READ

4. To exit:
   - In the client window: type  
       exit  
   - In the server window: press  
       Ctrl + C  

Troubleshooting:
- **Connection refused** → Ensure `server.exe` is already running in Window 1.  
- **Firewall prompt** → Allow “server.exe” through Windows Firewall.  
- **“WSAStartup error” or missing DLL** → Rare on modern Windows; verify you’re on Win10/11.

Enjoy—no compiler or IDE needed on the target PC!
