import 'dart:io';

class ServerConnectionHelper{
  bool connected = false;
  late Socket socket;
  int port = 8097;
  ServerConnectionHelper();
  late Function onDisconnectCallback;

  Future<void> connectToServer(String serverIP) async {
    await Socket.connect(serverIP, port).then((Socket sock){
      socket = sock;
      connected = true;
      socket.listen(
        dataHandler,
        onError: errorHandler,
        onDone: doneHandler,
        cancelOnError: true,
      );
    });
    return;
  }

  void sendData(data){
    if(connected){
      socket.write(data);
    }
  }

  void dataHandler(data){
  }

  void doneHandler(){
    connected = false;
    onDisconnectCallback();
    print("Done!");
  }

  void errorHandler(error, StackTrace trace){
    print(error);
    connected = false;
  }


  void registerDisconnectCallback(Function callback){
    onDisconnectCallback = callback;
  }


}