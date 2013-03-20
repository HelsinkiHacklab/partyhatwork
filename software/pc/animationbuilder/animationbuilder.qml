import QtQuick 1.1
//import QtUiComponents 1.0

Rectangle
{
    id: mainContainer
    width: 600; height: 400
    Column
    {
        width: parent.width; height: parent.height
        Rectangle
        {
            Text
            {
                text: "Foo"
            }
            /*
            Button
            {
                text: "Bar"
            }
            */
        }
    }
}

/*
Component
{
    id: animation_frame
    // TODO: the inputs
    // TODO2: where to get a color picker widget ??
}
*/

