#include <opencv2/opencv.hpp>

#include <iostream>
#include<queue>
#include<vector>


using namespace cv;
using namespace std;

int calculateAdjacentPixelValue(int pixel, int width, int i, int j){
    if(i==0 && j==0){
        return pixel - width;
    }
    else if(i==0 && j==1){
        return pixel + 1;
    }
    else if(i==1 && j==0){
        return pixel + width;
    }
    else
        return pixel - 1;
}

int checkIfPixelIsSink(list<int> sink, int pixel){
    for(int sinkPixel: sink){
        if(sinkPixel==pixel){
            return 1;
        }
    }
    return 0;
}

int findPixelType(Vec3b fg, Vec3b bg,Vec3b pixel){
    double distance1 = norm(fg, pixel, NORM_L2);
    double distance2 = norm(bg, pixel, NORM_L2);
    if(distance2>=distance1){
        return 1;
    }
    return 0;
}


int main( int argc, char** argv )
{
if(argc!=4){
    cout<<"Usage: ../seg input_image initialization_file output_mask"<<endl;
    return -1;
}

Mat in_image;
in_image = imread(argv[1], CV_LOAD_IMAGE_COLOR);

if(!in_image.data)
{
    cout<<"Could not load input image!!!"<<endl;
    return -1;
}

if(in_image.channels()!=3){
    cout<<"Image does not have 3 channels!!! "<<in_image.depth()<<endl;
    return -1;
}


Mat out_image = in_image.clone();
Mat clone_image = in_image.clone();


  int columns = in_image.cols;
  int rows = in_image.rows;




ifstream f(argv[2]);
if(!f){
    cout<<"Could not load initial mask file!!!"<<endl;
    return -1;
}

    
    int sumRF=0,sumRB=0,sumGF=0,sumGB=0,sumBF=0,sumBB=0;
    int n;
    f>>n;
    
    list<int> source, sink;
    
    for(int i=0;i<n;++i){
        int x, y, t;
        f>>x>>y>>t;
        
        if(x<0 || x>=columns || y<0 || y>=rows){
            cout<<"Invalid pixel mask!"<<endl;
            return -1;
        }

        if(t==0){
            sink.push_back(x + (columns * y));
            Vec3b pixel = clone_image.at<Vec3b>(y,x);
            sumBB+=(int)pixel[0];
            sumGB+=(int)pixel[1];
            sumRB+=(int)pixel[2];
        }
        else {
            source.push_back(x + (columns * y));
             Vec3b pixel = clone_image.at<Vec3b>(y,x);
             sumBF+=(int)pixel[0];
             sumGF+=(int)pixel[1];
             sumRF+=(int)pixel[2];
        }
        
    }

    Vec3b sPixel (sumBF/source.size(),sumGF/source.size(),sumRF/source.size());
    Vec3b tPixel (sumBB/sink.size(),sumGB/sink.size(),sumRB/sink.size());


 int*** weights = new int** [rows*columns];

 int c = 0;

 while(c<rows*columns){    
    int** subweights = new int* [2];
    weights[c] = subweights;
    c++;
    }

 c = 0;

 int d = 0;

 while(c<rows*columns){ 
    while (d < 2){
        weights[c][d] = new int [2];
        d++;
    }
    c++;
    d=0;
}


  for(int i=0;i<rows*columns;++i) {
    int down = i / columns;
    int sideways = i % columns;

     if(down == 0 && sideways == 0){
        weights[i][0][1] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down, sideways+1))?9999999:1);    //right edge weight
        weights[i][1][0] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down + 1, sideways))?9999999:1);  //down edge weight
     }

     else if (down == 0 && sideways == columns -1) {
        weights[i][1][0] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down + 1, sideways))?9999999:1); //down edge weight
        weights[i][1][1] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down, sideways - 1))?9999999:1);  //left edge weight
     }

     else if (down == rows - 1 && sideways == 0){
        weights[i][0][1] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down, sideways+1))?9999999:1);    //right edge weight
        weights[i][0][0] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down - 1, sideways))?9999999:1);  //up edge weight 
     }

     else if (down == rows -1 && sideways == columns -1) {
        weights[i][1][1] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down, sideways - 1))?9999999:1);    //left edge weight
        weights[i][0][0] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down - 1, sideways))?9999999:1);    //up edge weight
     }

     else if (down == 0) {
        weights[i][0][1] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down, sideways+1))?9999999:1);  //right edge weight
        weights[i][1][0] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down + 1, sideways))?9999999:1);  //down edge weight
        weights[i][1][1] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down, sideways - 1))?9999999:1);   //left edge weight
     }

     else if (sideways == columns - 1){
        weights[i][1][0] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down + 1, sideways))?9999999:1);   //down edge weight
        weights[i][1][1] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down, sideways - 1))?9999999:1);    //left edge weight
        weights[i][0][0] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down - 1, sideways))?9999999:1);    //up edge weigh
     }

     else if (sideways == 0) {
        weights[i][1][0] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down + 1, sideways))?9999999:1);   //down edge weight
        weights[i][0][1] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down, sideways+1))?9999999:1);     //right edge weight
        weights[i][0][0] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down - 1, sideways))?9999999:1);    //up edge weight
     }

     else if (down == rows -1) {
        weights[i][1][1] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down, sideways - 1))?9999999:1);    //left edge weight
        weights[i][0][0] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down - 1, sideways))?9999999:1);    //up edge weigh
        weights[i][0][1] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down, sideways+1))?9999999:1);     //right edge weight
     }

     else {
        weights[i][1][1] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down, sideways - 1))?9999999:1);   //left edge weight
        weights[i][0][0] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down - 1, sideways))?9999999:1);    //up edge weight
        weights[i][0][1] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down, sideways+1))?9999999:1);     //right edge weight
        weights[i][1][0] = (findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down,sideways))==findPixelType(sPixel, tPixel, clone_image.at<Vec3b>(down + 1, sideways))?9999999:1);    //down edge weight
     }
 }



 
    vector<int> visited;

    for(int i=0;i<rows*columns;++i){
        visited.push_back(0);
    }


    queue<vector<int>> allPaths;
    vector<int> mynewpath;

    for(int sourcePixel: source){
        mynewpath.push_back(sourcePixel);
        allPaths.push(mynewpath);
        mynewpath.clear();
    }



        while(!allPaths.empty()){
            vector<int> currentPath = allPaths.front();
            allPaths.pop();
            int latestPixel = currentPath.back();
            if(checkIfPixelIsSink(sink,latestPixel)) {
                int min=2147483647;
                for(int i=0;i<currentPath.size()-1;++i){
                    int diff = currentPath[i] - currentPath[i+1];
                    int weight=0;

                    if(diff==-1){
                        weight=weights[currentPath[i]][0][1];
                    }
                    else if(diff==1){
                        weight=weights[currentPath[i]][1][1];
                    }
                    else if (diff==columns){
                        weight=weights[currentPath[i]][0][0];
                    }
                    else {
                        weight=weights[currentPath[i]][1][0];
                    }
                    if(min>weight){
                        min=weight;
                    }
                }

                for(int i=0;i<currentPath.size()-1;++i) {
                    int diff = currentPath[i] - currentPath[i+1];
                    if(diff==-1){
                        weights[currentPath[i]][0][1]-=min;
                        weights[currentPath[i+1]][1][1]+=min;
                    }
                    else if(diff==1){
                        weights[currentPath[i]][1][1]-=min;
                        weights[currentPath[i+1]][0][1]+=min;
                    }
                    else if (diff==columns){
                        weights[currentPath[i]][0][0]-=min;
                        weights[currentPath[i+1]][1][0]+=min;
                    }
                    else {
                        weights[currentPath[i]][1][0]-=min;
                        weights[currentPath[i+1]][0][0]+=min;
                    }

                }



                currentPath.clear();

                visited.clear();
                for(int i=0;i<rows*columns;++i){
                    visited.push_back(0);
                }

                while(!allPaths.empty()){
                    allPaths.pop();
                }

                mynewpath.clear();
                
                for(int sourcePixel: source){
                    mynewpath.push_back(sourcePixel);
                    allPaths.push(mynewpath);
                    mynewpath.clear();
                }
            
            }

            else {
                for(int i=0;i<2;++i){
                    for(int j=0;j<2;++j){
                        if(weights[latestPixel][i][j]>0){
                                 if(visited[calculateAdjacentPixelValue(latestPixel,columns,i,j)]==0){
                                    vector<int> appendPath(currentPath);
                                    appendPath.push_back(calculateAdjacentPixelValue(latestPixel,columns,i,j));
                                    allPaths.push(appendPath);
                                    visited[calculateAdjacentPixelValue(latestPixel,columns,i,j)]=1;
                                 }
                        }
                    }
                }
            }
        }

    
        visited.clear();
        for(int i=0;i<rows*columns;++i){
            visited.push_back(0);
        }


    Vec3b blackPixel;
    blackPixel[0] = 0;
    blackPixel[1] = 0;
    blackPixel[2] = 0;

    for(int i=0;i<rows;++i){
        for(int j=0;j<columns;++j){
            out_image.at<Vec3b>(i,j) = blackPixel;
        }
    }


    Vec3b whitePixel;
    whitePixel[0] = 255;
    whitePixel[1] = 255;
    whitePixel[2] = 255;

    for(int sourcePixel: source){
        queue<int> q;
        q.push(sourcePixel);
        out_image.at<Vec3b>(sourcePixel/columns,sourcePixel%columns)=whitePixel;
        visited[sourcePixel]=1;
        while(!q.empty()){
            int u = q.front();
            q.pop();
            for(int j=0;j<2;++j){
                for(int k=0;k<2;++k){
                    if(weights[u][j][k]>0){
                        int adjacentPixel = calculateAdjacentPixelValue(u,columns,j,k);
                        if(visited[adjacentPixel]==0){
                            out_image.at<Vec3b>(adjacentPixel/columns,adjacentPixel%columns) = whitePixel;
                            q.push(adjacentPixel);
                            visited[adjacentPixel]=1;
                        }
                    }
                }
            }
        }
    }
    


    //write it on disk
    imwrite( argv[3], out_image);


 

    namedWindow( "Original image", WINDOW_AUTOSIZE );
    namedWindow( "Show Marked Pixels", WINDOW_AUTOSIZE );
    imshow( "Original image", in_image );
    imshow( "Show Marked Pixels", out_image );

    waitKey(0);
    return 0;
}

