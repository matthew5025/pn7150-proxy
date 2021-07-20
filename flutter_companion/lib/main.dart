import 'package:flutter/material.dart';
import 'package:flutter_companion/ConnectionPage.dart';
import 'package:flutter_companion/ServerConnectionHelper.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  final appTitle = 'Drawer Demo';

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: appTitle,
      home: MyHomePage(),
    );
  }
}

class MyHomePage extends StatefulWidget {
  final String title = "Test";

  @override
  _MyHomePageState createState() => new _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  int _selectedDrawerIndex = 0;
  ServerConnectionHelper serverConnectionHelper = new ServerConnectionHelper();

  _getDrawerItemWidget(int pos) {
    switch (pos) {
      case 0:
        return new ConnectionPage(scHelper: serverConnectionHelper,);
      case 1:
        return new ConnectionPage(scHelper: serverConnectionHelper,);
      case 2:
        return new ConnectionPage(scHelper: serverConnectionHelper,);

      default:
        return new Text("Error");
    }
  }

  @override
  Widget build(BuildContext context) {
    return new Scaffold(
      appBar: new AppBar(
        title: new Text(widget.title),
      ),
      drawer: new Drawer(
        child: new ListView(
          padding: EdgeInsets.zero,
          children: <Widget>[
            new DrawerHeader(
              decoration: BoxDecoration(
                color: Colors.blue,
              ),
              child: Text('Drawer Header'),
            ),
            new ListTile(
                title: new Text('Registration Search'),
                onTap: () => _onSelectItem(0)),
            new ListTile(
                title: new Text('Service Search'),
                onTap: () => _onSelectItem(1)),
            new ListTile(
              title: new Text("Model Search"),
              onTap: () => _onSelectItem(2),
            )
          ],
        ),
      ),
      body: _getDrawerItemWidget(_selectedDrawerIndex),
    );
  }

  _onSelectItem(int index) {
    setState(() => _selectedDrawerIndex = index);
    Navigator.of(context).pop(); // close the drawer
  }
}
