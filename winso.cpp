#pragma once
#include <iostream>
#include "WinSock2.h"//importamos la libreria Winsock
using namespace std;

const int STRLEN = 256;

class Socket //definimos la clase Socket
{
    protected://para que las siguientes clases puedan heradar
        WSADATA wsaData;
        SOCKET mySocket;
        SOCKET myBackup;
        SOCKET acceptSocket;
        sockaddr_in myAddress;
    public:
        Socket();//constructor
        ~Socket();//destructor
        bool SendData( char* );//fuinciones que se definiran luego
        bool RecvData( char*, int );
        void CloseConnection();
        void GetAndSendMessage();
};

class ServerSocket : public Socket//hereda las variables de socket
{
    public:
        void Listen();//fuinciones que se definiran luego
        void Bind( int port );
        void StartHosting( int port );
};

class ClientSocket : public Socket//hereda las variables de socket
{
    public://fuinciones que se definiran luego
        void ConnectToServer( const char *ipAddress, int port );
};
Socket::Socket()//inicializacion del socket
{
    if( WSAStartup( MAKEWORD(2, 2), &wsaData ) != NO_ERROR )
    {
        cerr<<"Socket Initialization: Error con WSAStartup\n";
        system("pause");
        WSACleanup();
        exit(10);
    }

   //creando el socket
    mySocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( mySocket == INVALID_SOCKET )
    {
        cerr<<"Socket Initialization: Error Creando el  socket"<<endl;
        system("pause");
        WSACleanup();
        exit(11);
    }

    myBackup = mySocket;
}
//destructor si falla limpia la pantalla
Socket::~Socket()
{
    WSACleanup();
}
//envia datos
bool Socket::SendData( char *buffer )
{
    send( mySocket, buffer, strlen( buffer ), 0 );
    return true;
}
//recive datos
bool Socket::RecvData( char *buffer, int size )
{
    int i = recv( mySocket, buffer, size, 0 );
    buffer[i] = '\0';
    return true;
}
//cierra la coneccion
void Socket::CloseConnection()
{
    //cout<<"CLOSE CONNECTION"<<endl;
    closesocket( mySocket );
    mySocket = myBackup;
}

void Socket::GetAndSendMessage()
{
    char message[STRLEN];
    cin.ignore();//ignora el anterior char
    cout<<"Send > ";
    cin.get( message, STRLEN );
    SendData( message );
}

void ServerSocket::StartHosting( int port )
{//define el puerto
     Bind( port );
     Listen();
}

void ServerSocket::Listen()
{
    //cout<<"LISTEN FOR CLIENT..."<<endl;

    if ( listen ( mySocket, 1 ) == SOCKET_ERROR )
    {
        cerr<<"ServerSocket: Error listening on socket\n";
        system("pause");
        WSACleanup();
        exit(15);
    }


    acceptSocket = accept( myBackup, NULL, NULL );
    while ( acceptSocket == SOCKET_ERROR )
    {
        acceptSocket = accept( myBackup, NULL, NULL );
    }
    mySocket = acceptSocket;
}

void ServerSocket::Bind( int port )
{
    myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = inet_addr( "0.0.0.0" );
    myAddress.sin_port = htons( port );
//variables puerto tipo de conneccion y direccion IP

    if ( bind ( mySocket, (SOCKADDR*) &myAddress, sizeof( myAddress) ) == SOCKET_ERROR )
    {
        cerr<<"ServerSocket: Fallo La coneccion\n";
        system("pause");
        WSACleanup();
        exit(14);
    }
}

void ClientSocket::ConnectToServer( const char *ipAddress, int port )//Defines las coneccion del servidor y sus su puertos
{
    myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = inet_addr( ipAddress );
    myAddress.sin_port = htons( port );



    if ( connect( mySocket, (SOCKADDR*) &myAddress, sizeof( myAddress ) ) == SOCKET_ERROR )//si no conecta manda error y cierra la aplicacion y limpia la pantalla
    {
        cerr<<"ClientSocket: Fallo La coneccion\n";//se usa para imprimir errores
        system("pause");
        WSACleanup();
        exit(13);
    }
}
int main()
{
    int choice;
    int port = 666;
    string ipAddress;
    bool done = false;
    char recMessage[STRLEN];
    char sendMessage[STRLEN];
    cout<<"1.-Servidor"<<endl;
    cout<<"2.-Seleccionar servidor"<<endl;
    cout<<"3.-Salir"<<endl;
    cin>>choice;
    while (choice>3){
    cout<<"1.-Servidor"<<endl;
    cout<<"2.-Seleccionar servidor"<<endl;
    cout<<"3.-Salir"<<endl;
        cin>>choice;
    }
    if ( choice == 3 )
        exit(0);
    else if ( choice == 2 )
    {
        //Client
        cout<<"Ingresa una direccion IP pref 127.0.0.1"<<endl;
        cin>>ipAddress;
        ClientSocket sockClient;
        cout<<"Conectando...."<<endl;
        sockClient.ConnectToServer( ipAddress.c_str(), port );
        //Connected
        while ( !done )
        {
            sockClient.GetAndSendMessage();
            cout<<"\tVisto :v"<<endl;
            sockClient.RecvData( recMessage, STRLEN );
            cout<<"Recv > "<<recMessage<<endl;
            if ( strcmp( recMessage, "end" ) == 0 ||
                        strcmp( sendMessage, "end" ) == 0 )
            {
                done = true;
            }
        }
        sockClient.CloseConnection();
    }
    else if ( choice == 1 )
    {
        //SERVER
        ServerSocket sockServer;
        cout<<"Server..."<<endl;
        sockServer.StartHosting( port );
        //conectado
        while ( !done )
        {
            cout<<"\tVisto :v"<<endl;
            sockServer.RecvData( recMessage, STRLEN );
            cout<<"Recv > "<<recMessage<<endl;
            sockServer.GetAndSendMessage();
            if ( strcmp( recMessage, "end" ) == 0 ||
                    strcmp( sendMessage, "end" ) == 0 )
            {
                done = true;
            }
        }
    }
}
