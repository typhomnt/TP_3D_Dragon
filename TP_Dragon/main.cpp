# include "glCheck.h"
#include "viewer.h"
#include "dragon.h"

# include <iostream>
#include <qapplication.h>
int main(int argc, char** argv)
{
	// Read command lines arguments.
	QApplication application(argc,argv);

    // Instantiate the viewer
    QGLFormat format;
    //format.setVersion( 2, 1 );
    //format.setProfile( QGLFormat::CompatibilityProfile );

    // Instantiate the viewer.
    Viewer viewer( format );
    viewer.setSceneRadius(80.0f);
	// build your scene here
    viewer.addRenderable(new Dragon());

	viewer.setWindowTitle("viewer");
	// Make the viewer window visible on screen.
	viewer.show();

	// Run main loop.
	return application.exec();
}
