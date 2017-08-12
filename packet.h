class Packet{

	public:
		//Packet(PointCloud<PointXYZRGB> cloud);
		//PointCloud<PointXYZRGB> receivedCloud;

		Packet(PointXYZRGB, float, float, float, float, int, int);

		int packet_number;
		int total_packets;
		PointXYZRGB point;

		float x;
		float y;
		float z;
		float rgb;

};
