import 'dart:async';

import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:flutter_companion/ServerConnectionHelper.dart';

class ConnectionPage extends StatelessWidget {
  final ServerConnectionHelper scHelper;
  ConnectionPage({required this.scHelper});
  @override
  Widget build(BuildContext context) {
    return ConnectionStatefulPage(scHelper: scHelper,);
  }
}

class ConnectionStatefulPage extends StatefulWidget {
  final ServerConnectionHelper scHelper;
  ConnectionStatefulPage({required this.scHelper});

  @override
  State<StatefulWidget> createState() => _ConnectionPageState(scHelper);
}

class _ConnectionPageState extends State<ConnectionStatefulPage> {
  final ServerConnectionHelper scHelper;
  Widget mainView = new Text("Loading");
  _ConnectionPageState(this.scHelper){
    this.mainView = new _ConnectionPageDefaultWidget(scHelper: scHelper);
  }

  void _updateMainView() {
    setState(() {
      mainView = new _ConnectionPageDefaultWidget(scHelper: scHelper);
    });
  }

  @override
  Widget build(BuildContext context) {
    _onSearchButtonPressed() async {
      final result = await _createSearchDialog(context);
    }

    return new Scaffold(
      body: mainView,
      floatingActionButton: new FloatingActionButton(
          onPressed: () async {
            final ipAddr = await _createSearchDialog(context);
            print(ipAddr);
            if(ipAddr != null){
              _createConnectingDialog(context);
              await scHelper.connectToServer(ipAddr);
              print(scHelper.connected);
              scHelper.registerDisconnectCallback(_updateMainView);
              _updateMainView();
              Navigator.pop(context);
            }
          },
          child: new Icon(Icons.search)),
    );
  }
}


class _ConnectionPageDefaultWidget extends StatelessWidget {
  final ServerConnectionHelper scHelper;
  _ConnectionPageDefaultWidget({required this.scHelper});

  Card _getServerStatusCard(){

    String serverStatusText = "";
    if (scHelper.connected){
      serverStatusText = "Connected!";
    }else{
      serverStatusText = "Disconnected";

    }

    Card card = Card(
      clipBehavior: Clip.antiAlias,
      child: Column(
        children: [
          ListTile(
            title: Text("Server Status"),
            subtitle: Text(serverStatusText),
          )
        ],
      ),
    );
    return card;
  }

  @override
  Widget build(BuildContext context) {
    return new Container(
      margin: EdgeInsets.all(4.0),
      child: ListView(
        children: [
          _getServerStatusCard(),
        ],
      )
    );
  }
}

Future _createSearchDialog(BuildContext context) async {
  final textEditingController = new TextEditingController();
  String? ipAddr = await showDialog<String>(
      context: context,
      builder: (BuildContext context) {
        return new SimpleDialog(
          title: const Text('Server Connection'),
          contentPadding: EdgeInsets.symmetric(horizontal: 20),
          children: <Widget>[
            new Form(
              child: Column(
                children: [
                  TextFormField(
                    decoration: InputDecoration(
                      labelText: 'Server IP Address',
                    ),
                    controller: textEditingController,
                  ),
                  ButtonBar(
                    children: [
                      ElevatedButton(
                        onPressed: () {
                          Navigator.pop(context, textEditingController.text);
                        },
                        child: Text('Connect'),
                      )
                    ],
                  )
                ],
              ),
            )
          ],
        );
      });
  return ipAddr;
}

_createConnectingDialog(BuildContext context) {
  showDialog(
      context: context,
      barrierDismissible: false,
      builder: (BuildContext context) {
        return new AlertDialog(
          title: new Text('Please Wait'),
          content: new Form(
            child: new Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: <Widget>[
                CircularProgressIndicator(),
                new Text('Connecting. . .'),
              ],
            ),
            onWillPop: _onWillPop,
          ),
        );
      });
}

_createErrorDialog(BuildContext context, String text) {
  showDialog(
      context: context,
      barrierDismissible: true,
      builder: (BuildContext context) {
        return new AlertDialog(
          title: new Text('Error!'),
          content: new Text('$text'),
          actions: <Widget>[
            new FlatButton(
                onPressed: () {
                  Navigator.of(context).pop();
                },
                child: new Text('Ok'))
          ],
        );
      });
}

Future<bool> _onWillPop() async {
  var completer = new Completer();
  completer.complete(false);
  if (await completer.future == null) {
    return false;
  } else {
    return await completer.future;
  }
}
