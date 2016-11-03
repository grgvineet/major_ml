#include <bits/stdc++.h>
#define MAX_POINTS 100
using namespace std;

bool definitelyGreaterThan(double a, double b, double epsilon = 1e-6)
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

double point_distance(double x1, double y1, double x2, double y2)
{
	return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

int main()
{
	int num_points, k, cluster[MAX_POINTS], centroid_count[MAX_POINTS];
	double px[MAX_POINTS], py[MAX_POINTS], cx[MAX_POINTS], cy[MAX_POINTS];
	double min_dist, distance, centroid_x[MAX_POINTS], centroid_y[MAX_POINTS];  

	cout << "Enter number of points" << endl;
	cin >> num_points;

	cout << "Enter number of clusters (i.e. k)" << endl;
	cin >> k;

	cout << "Enter the points" << endl;
	for(int i=0; i<num_points; i++)
	{
		cin >> px[i] >> py[i];
		cluster[i] = -1;
	}

	// intialising cluster points as data points
	for(int i=0; i<k ;i++)
	{
		cx[i] = px[i];
		cy[i] = py[i];
	}

	int max_iterations = 1e6, iter = 0;
	bool flag = true;

	while(flag && ++iter < max_iterations)
	{
		flag = false;

		for(int i=0; i<num_points; i++)
		{
			min_dist = point_distance(px[i], py[i], cx[0], cy[0]);
			int cp = 0;

			for(int j=1; j<k; j++)
			{
				distance = point_distance(px[i], py[i], cx[j], cy[j]);
				if(definitelyGreaterThan(min_dist, distance))
				{
					min_dist = distance;
					cp = j;
				}
			}

			if(cp != cluster[i])
			{
				flag = true;
				cluster[i] = cp;
			}
		}

		memset(centroid_count,0,sizeof(centroid_count));
		memset(centroid_x,0,sizeof(centroid_x));
		memset(centroid_y,0,sizeof(centroid_y));

		for(int i=0; i<num_points; i++)
		{
			centroid_x[cluster[i]] += px[i];
			centroid_y[cluster[i]] += py[i];
			centroid_count[cluster[i]] ++;
		}

		for(int i=0; i<k; i++)
		{
			if(centroid_count[i])
			{
				cx[i] = centroid_x[i]/centroid_count[i];
				cy[i] = centroid_y[i]/centroid_count[i];
			}
		}
	}

	for(int i=0;i<k;i++)
	{
		cout << "Cluster " << i+1 << " : " << cx[i] << "," << cy[i] << endl;
	}

	return 0;
}