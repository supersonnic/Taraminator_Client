/*
client example using cpp taken from http://www.cplusplus.com/forum/unices/116977/

This implementation is only valid on unix environments. This can be changed to check for windows.

*/

#include <string.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>

#include "client_viewer.h"
#include "packet.h"

#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>

#include <pcl/point_types.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/io/io.h>
#include <pcl/filters/voxel_grid.h>

using namespace pcl;
using namespace std;

#define DEFAULT_FRAME_WIDTH		640
#define DEFAULT_FRAME_HEIGHT		480



int main (int argc, char* argv[])
{
    int listenFd, portNo;
    bool loop = false;
    struct sockaddr_in svrAdd;
    struct hostent *server;



    if(argc < 3)
    {
        cerr<<"Syntax : ./client <host name> <port>"<<endl;
        return 0;
    }

    portNo = atoi(argv[2]);

    if((portNo > 65535) || (portNo < 2000))
    {
        cerr<<"Please enter port number between 2000 - 65535"<<endl;
        return 0;
    }

    //create client skt
    listenFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(listenFd < 0)
    {
        cerr << "Cannot open socket" << endl;
        return 0;
    }

    server = gethostbyname(argv[1]);

    if(server == NULL)
    {
        cerr << "Host does not exist" << endl;
        return 0;
    }

    bzero((char *) &svrAdd, sizeof(svrAdd));
    svrAdd.sin_family = AF_INET;

    bcopy((char *) server -> h_addr, (char *) &svrAdd.sin_addr.s_addr, server -> h_length);

    svrAdd.sin_port = htons(portNo);

    int checker = connect(listenFd,(struct sockaddr *) &svrAdd, sizeof(svrAdd));

    if (checker < 0)
    {
        cerr << "Cannot connect!" << endl;
        return 0;
    }



/*
	read data coming from socket. Each frame will reset the point cloud stream per frame.
*/

	cout << "Waiting for data" <<endl;

	// setup client viewer

	//Viewer viewer;
	PointCloud<PointXYZRGB> receivedCloud;
	PointCloud<PointXYZRGB>::Ptr receivedCloud_ptr (new PointCloud<PointXYZRGB>);
	visualization::CloudViewer cloudViewer ("Cloud Viewer");

	VoxelGrid<PointXYZRGB> vg;
	PointCloud<PointXYZRGB>::Ptr cloud_filtered(new PointCloud<PointXYZRGB>);


	bool cloudViewerStatus = false; // not started true means it's running
	int number_of_points;
	cout << "Please enter number of points" <<endl;
	scanf("%d", &number_of_points);
	int count = 0;
	float max=0;
    //receive stuff from server
    for(;;)
    {
	//printf("%f", &max);

        //read(listenFd, s[DEFAULT_FRAME_WIDTH * DEFAULT_FRAME_HEIGHT], DEFAULT_FRAME_WIDTH * DEFAULT_FRAME_HEIGHT);
	// example from https://stackoverflow.com/questions/3261227/how-to-serialize-an-object-to-send-over-network
	// http://www.functionx.com/cpp/articles/serialization.htm

	//std::cout<<("displaying text" + sizeof(Packet));
        //int buf[sizeof(PointCloud<PointXYZRGB>::Ptr)];

	// when increasing the buffer size, the frame rate increased? not sure how that works.
	int buf[sizeof(PointXYZRGB)];
        read(listenFd, buf, sizeof(PointXYZRGB));
        PointXYZRGB *receivedPacket = reinterpret_cast<PointXYZRGB*>(buf);

	PointXYZRGB point = *receivedPacket;

	if(count <= number_of_points)
	{
		//if(max<=recievedCloud.point.x){max=recievedCloud.point.x;}
		receivedCloud.points.push_back(point);
		//cloudViewer.showCloud(receivedCloud.makeShared());
		count ++;
	}
	else{
		count=0;

		//http://www.pcl-users.org/VoxelGridFilter-Leaf-size-is-too-small-td4025570.html
		// "Leaf size is too small for the input dataset. Integer indices would overflow"
		// appears when applying voxel filter.

		//vg.setInputCloud( receivedCloud.makeShared() );
		//vg.setLeafSize( 0.5, 0.5, 0.5 );
		//vg.filter(*cloud_filtered);

		cloudViewer.showCloud(receivedCloud.makeShared());

		// saving file to load into meshlabs
		//std::string writePath = "";
		//pcl::io::savePCDFileASCII ("camera_pcd.pcd", receivedCloud);


		//cout << "clearing cloud..." <<endl;
		receivedCloud.clear();

	}


/*
	// reconstruct xyz point
	PointXYZRGB point(receivedPacket->x, receivedPacket->y, receivedPacket->z);
	point.rgb= receivedPacket->rgb;

	// add point to cloud
	PointXYZRGB point = *receivedPacket;
	receivedCloud.push_back(point);

	PointCloud<PointXYZRGB>::Ptr cloud_ptr (new PointCloud<PointXYZRGB>);
	//cloud_ptr = receivedPacket->makeShared();
*/
	//      cout << "showing cloud...." << endl;
	//cloudViewer.showCloud(temp);




	//viewer.showViewer(receivedCloud);
    }
}


