#include <iostream>
#include <fstream>
#include <list>
#include <cmath>

using namespace std;

class Vertex { // class that represents a vertex (i.e. customer)
  int ix;
  int x;
  int y;
  int demand;
  int readyTime;
  int dueDate;
  int serviceTime;
  int arrivalTime;
  int serviceStartTime;
  int serviceCompletionTime;
  public:
  Vertex() {
    this->ix = 0;
    this->x = 0;
    this->y = 0;
    this->demand = 0;
    this->readyTime = 0;
    this->dueDate = 0;
    this->serviceTime = 0;
    this->arrivalTime = 0;
    this->serviceStartTime = 0;
    this->serviceCompletionTime = 0;
  }
  Vertex(int ix, int x, int y, int demand, int readyTime, int dueDate, int serviceTime) {
    this->ix = ix;
    this->x = x;
    this->y = y;
    this->demand = demand;
    this->readyTime = readyTime;
    this->dueDate = dueDate;
    this->serviceTime = serviceTime;
    this->arrivalTime = 0;
    this->serviceStartTime = 0;
    this->serviceCompletionTime = 0;
  }
  int getX() { return x; }
  int getY() { return y; }
  int getDemand() { return demand; }
  int getReadyTime() { return readyTime; }
  int getDueDate() { return dueDate; }
  int getServiceTime() { return serviceTime; }
  int getArrivalTime() { return arrivalTime; }
  int getServiceStartTime() { return arrivalTime; }
  int getServiceCompletionTime() {return departureTime; }
  void setArrivalTime(int value) { arrivalTime = value; }
  void setServiceStartTime(int value) {serviceStartTime = value; }
  void setServiceCompletionTime(int value) {serviceCompletionTime = value; }  
  void offsetArrivalTime(int by) { arrivalTime += by; }
  void offsetServiceStartTime(int by) { serviceStartTime += by; }
  void offsetServiceCompletionTime(int by) {serviceCompletionTime += by; }
  friend ostream& operand<<(ostream& out, Vertex vertex) {
    
  }
  void printVertex() {
    cout << "Vertex #" << ix << endl;
    cout << "x-coordinate: " << x << endl;
    cout << "y-coordinate: " << y << endl;
    cout << "demand:       " << demand << endl;
    cout << "ready time:   " << readyTime << endl;
    cout << "due date:     " << dueDate << endl;
    cout << "service time: " << serviceTime << endl;
  }
};

int d(Vertex* v_i, Vertex* v_j) {
  return sqrt(pow(v_i->getX() - v_j->getX(), 2) + pow(v_i->getY() - v_j->getY(), 2));
}

int pe(Vertex* v) {
  return max(0, v->getReadyTime() - v->getArrivalTime());
}

int pl(Vertex* v) {
  return max(0, v->getDepartureTime() - v->getDueDate());
}

class Route {
  int totalTime;
  int totalCost;
  int remainingCapacity;
  Vertex* depot;
  int getRemainingCapacity() { return this->remainingCapacity; }
  list<Vertex*> vertices;
  public:
  Route(Vertex* depot) {
    this->depot = depot;
  }
  Route(Vertex* vertex, Vertex* depot) {
    this->depot = depot;
    this->addVertexToRoute(vertex);
  }
  Route(list<Vertex*> vertices, Vertex* depot) {
    this->depot = depot;
    for (Vertex vertex : vertices) this->addVertexToRoute(vertex);
  }
  void addVertexToRoute(Vertex* v) {
    Vertex* lastCustomer = this->vertices.back();
    v->setArrivalTime(lastCustomer->getServiceCompletionTime() + d(v, vertices.back()));
    v->setServiceStartTime(max(lastCustomer->getServiceCompletionTime() + d(v, lastCustomer), v->getReadyTime()));
    v->setDepartureTime(v->getServiceStartTime() + v->getServiceTime());
    this->totalTime += d(vertices.back(), v) 
                    + v.getServiceTime() 
                    + d(v, depot) 
                    - d(vertices.back(), v);
    this->totalCost += d(vertices.back(), v) 
                    + v.getServiceTime() 
                    + d(v, depot) 
                    - d(vertices.back(), v) 
                    + pe(v) 
                    + pl(v);
    this->vertices.push_back(v);
    this->remainingCapacity -= v->getDemand();
  }
};

int calculateTimeSaving(Route &r1, Route &r2) {
  Vertex* v1 = r1.vertices.back();
  Vertex* v2 = r2.vertices.back();
  
  int timeSaving = d(v1, depot) 
                 + max(d(route.depot, v2), v2.getReadyTime()) 
                 - max(max(d(v1, route.depot), v1.getReadyTime()) + v1.getServiceTime(), v2.getReadyTime());

  return timeSaving + earlyTimePenaltySaving + lateTimePenaltySaving;
}

int calculateTimeSaving(Route &route, Vertex* v) {
  Vertex* v1 = route.vertices.back();
  Vertex* v2 = v;
  
  int timeSaving = d(v1, depot) 
                 + max(d(route.depot, v2), v2.getReadyTime()) 
                 - max(max(d(v1, route.depot), v1.getReadyTime()) + v1.getServiceTime(), v2.getReadyTime());

  return timeSaving;
}

int calculateCostSaving(Route &r1, Route &r2) {
  Vertex* v1 = r1.vertices.back();
  Vertex* v2 = r2.vertices.back();

  int earlyTimePenaltySaving = max(0, v1->getDepartureTime() + d(*v1, *v2) + v1->getServiceTime() - v2->getDueDate())
    - max(0, d(r2.depot, v2) + v2.getServiceTime() - v2.getDueDate());

  int lateTimePenaltySaving = max(0, v1.getDepartureTime() + d(v1, v2) + v2.getServiceTime() - v2.getDueDate())
    - max(0, d(r2.depot, v2) + v2.getServiceTime() - v2.getDueDate());

  int timeSaving = d(v1, depot) + d(r2.depot, v2) - d(v1, v2);

  return timeSaving + earlyTimePenaltySaving + lateTimePenaltySaving;
}

class VRPProblem { // class representing an instance of Solomon Benchmark problems
  const char * file;
  bool header;
  public:
  VRPProblem(const char * file, bool header) { // constructor that uses the file path to retrieve the problem parameters
    this->file = file;
    this->header = header;
  }
  list<Vertex> getVertices() {
    int ix[101]; // indices of the vertices
    int x[101]; // x-coordinate of the vertices
    int y[101]; // y-coordinate of the vertices
    int demand[101]; // demand at each vertex
    int readyTime[101]; // time the service can start for each vertex
    int dueDate[101]; // time before the service should be completed for each vertex
    int serviceTime[101]; // time it takes to serve a vertex
    ifstream reader(file, ifstream::in);
    string data;
    int count = 0;
    if (header) getline(reader,data,'\n');
    while (getline(reader,data,'\t')) {
      ix[count] = atoi(data.c_str());
      getline(reader,data,'\t');
      x[count] = atoi(data.c_str());
      getline(reader,data,'\t');
      y[count] = atoi(data.c_str());
      getline(reader,data,'\t');
      demand[count] = atoi(data.c_str());
      getline(reader,data,'\t');
      readyTime[count] = atoi(data.c_str());
      getline(reader,data,'\t');
      dueDate[count] = atoi(data.c_str());
      getline(reader,data,'\n');
      serviceTime[count] = atoi(data.c_str());
      count++;
    }
    list<Vertex> vertices;
    for (int i = 0; i < count; i++) {
      vertices.emplace_back(ix[i], x[i], y[i], demand[i], readyTime[i], dueDate[i], serviceTime[i]);
    }
    return vertices;
  }
};

void mergeRoutes(Route r1, Route r2) { // merges two routes by inserting r2 to the tail of r1
  
}

list<Route> buildInitialRoutes(list<Vertex> vertices) {
  list<Route> routes;
  Vertex depot = vertices.front();
  list<Vertex>::iterator it = vertices.begin();
  it++;
  while (it != vertices.end()) {
    routes.push_back(Route(*it, depot));
  }
  return routes;
}

int main(int argc, char const *argv[])
{
  list<Vertex*> l;
  Vertex *v = new Vertex(0,1,2,3,4,5,6);
  l.push_back(v);
  l.front()->printVertex();
  // const char * file = "c101_tab_delim.txt";
  // VRPProblem problem = VRPProblem(file, true);
  // list<Vertex> vertices = problem.getVertices();
  // 
  // list<Route> routes = buildInitialRoutes(vertices);
  
  map<pair<int, Route*>> savings;
  
  for (unsigned int i = 0; i < routes.size(); i++) {
    for (unsigned int i = 0; j < i; j++) {
      int saving = make_pair<pair<Route*, Route*>, int>(make_pair<Route*, Route*>(i, j), calculateCostSaving(routes[i], routes[j]));
    }
  }
  
  while (savings.size() != 0) {
    auto saving = savings.end();
    if (saving.second.first->getRemainingCapacity() >= saving.second.second->getDeamnd()) {
      saving.second.first.mergeRoutes(saving.second.second);
    } else {
      saving--;
    }
  }
  
  cin.get();
  return 0;
}
