//#include <SFML/Graphics.hpp>
//#include <SFML/Window.hpp>
//#include "Graph.h"
//#include "DataLoader.h"
//#include "TransportManager.h"
//#include "EducationSector.h"
//#include "MedicalSector.h"
//#include "MallManager.h"
//#include "FacilityManager.h"
//#include "AirportManager.h"
//#include "RailwayManager.h"
//#include <iostream>
//#include <cmath>
//#include <string>
//using namespace std;
//
//const double M_PI = 3.14159265358979323846;
//
//// Color definitions for different vertex types
//const sf::Color COLOR_BUS_STOP = sf::Color(100, 150, 255);      // Light blue
//const sf::Color COLOR_HOSPITAL = sf::Color(255, 100, 100);      // Light red
//const sf::Color COLOR_SCHOOL = sf::Color(100, 255, 100);       // Light green
//const sf::Color COLOR_MALL = sf::Color(255, 200, 100);         // Orange
//const sf::Color COLOR_PHARMACY = sf::Color(255, 100, 255);     // Magenta
//const sf::Color COLOR_FACILITY = sf::Color(200, 200, 200);     // Gray
//const sf::Color COLOR_AIRPORT = sf::Color(255, 255, 100);      // Yellow
//const sf::Color COLOR_RAILWAY = sf::Color(150, 100, 255);      // Purple
//const sf::Color COLOR_EDGE = sf::Color(150, 150, 150);         // Gray
//const sf::Color COLOR_TEXT = sf::Color::White;
//const sf::Color COLOR_BACKGROUND = sf::Color(20, 20, 30);       // Dark blue-gray
//const sf::Color COLOR_HOVER = sf::Color(255, 255, 0);           // Yellow
//
//// Helper function to parse coordinates from string "lat, lon"
//static bool parseCoordinates(const string& coordStr, double& lat, double& lon) {
//    if (coordStr.empty()) return false;
//    
//    string trimmed = coordStr;
//    if (trimmed.length() >= 2 && trimmed[0] == '"' && trimmed[trimmed.length() - 1] == '"') {
//        trimmed = trimmed.substr(1, trimmed.length() - 2);
//    }
//    
//    size_t commaPos = trimmed.find(',');
//    if (commaPos == string::npos) return false;
//    
//    string latStr = trimmed.substr(0, commaPos);
//    string lonStr = trimmed.substr(commaPos + 1);
//    
//    // Trim whitespace
//    size_t start = 0, end = latStr.length() - 1;
//    while (start < latStr.length() && (latStr[start] == ' ' || latStr[start] == '\t')) start++;
//    while (end > start && (latStr[end] == ' ' || latStr[end] == '\t')) end--;
//    if (start <= end) latStr = latStr.substr(start, end - start + 1);
//    
//    start = 0; end = lonStr.length() - 1;
//    while (start < lonStr.length() && (lonStr[start] == ' ' || lonStr[start] == '\t')) start++;
//    while (end > start && (lonStr[end] == ' ' || lonStr[end] == '\t')) end--;
//    if (start <= end) lonStr = lonStr.substr(start, end - start + 1);
//    
//    // Convert to double (simple conversion)
//    lat = 0.0; lon = 0.0;
//    bool hasDecimal = false;
//    double fraction = 0.1;
//    
//    // Parse latitude
//    bool isNegative = false;
//    int i = 0;
//    if (!latStr.empty() && latStr[0] == '-') {
//        isNegative = true;
//        i = 1;
//    }
//    for (; i < (int)latStr.length(); i++) {
//        if (latStr[i] >= '0' && latStr[i] <= '9') {
//            if (!hasDecimal) {
//                lat = lat * 10.0 + (latStr[i] - '0');
//            } else {
//                lat += (latStr[i] - '0') * fraction;
//                fraction *= 0.1;
//            }
//        } else if (latStr[i] == '.' && !hasDecimal) {
//            hasDecimal = true;
//        }
//    }
//    if (isNegative) lat = -lat;
//    
//    // Parse longitude
//    hasDecimal = false;
//    fraction = 0.1;
//    isNegative = false;
//    i = 0;
//    if (!lonStr.empty() && lonStr[0] == '-') {
//        isNegative = true;
//        i = 1;
//    }
//    for (; i < (int)lonStr.length(); i++) {
//        if (lonStr[i] >= '0' && lonStr[i] <= '9') {
//            if (!hasDecimal) {
//                lon = lon * 10.0 + (lonStr[i] - '0');
//            } else {
//                lon += (lonStr[i] - '0') * fraction;
//                fraction *= 0.1;
//            }
//        } else if (lonStr[i] == '.' && !hasDecimal) {
//            hasDecimal = true;
//        }
//    }
//    if (isNegative) lon = -lon;
//    
//    return true;
//}
//
//// Map latitude/longitude to screen coordinates with zoom and pan
//sf::Vector2f mapToScreen(double lat, double lon, double minLat, double maxLat, 
//                         double minLon, double maxLon, int screenWidth, int screenHeight,
//                         float zoomLevel = 1.0f, float panX = 0.0f, float panY = 0.0f) {
//    // Normalize coordinates to 0-1 range
//    double normalizedLat = (lat - minLat) / (maxLat - minLat);
//    double normalizedLon = (lon - minLon) / (maxLon - minLon);
//    
//    // Calculate center of screen
//    float centerX = screenWidth / 2.0f;
//    float centerY = screenHeight / 2.0f;
//    
//    // Map to screen with zoom and pan
//    float padding = 80.0f;
//    float baseX = padding + normalizedLon * (screenWidth - 2 * padding);
//    float baseY = padding + (1.0 - normalizedLat) * (screenHeight - 2 * padding); // Flip Y axis
//    
//    // Apply zoom (scale around center)
//    float x = centerX + (baseX - centerX) * zoomLevel;
//    float y = centerY + (baseY - centerY) * zoomLevel;
//    
//    // Apply pan
//    x += panX;
//    y += panY;
//    
//    return sf::Vector2f(x, y);
//}
//
//// Determine vertex color based on ID
//sf::Color getVertexColor(const string& vertexID) {
//    if (vertexID.find("Stop") == 0) return COLOR_BUS_STOP;
//    if (vertexID.find("Hospital") == 0 || vertexID.find("H") == 0) return COLOR_HOSPITAL;
//    if (vertexID.find("School") == 0 || vertexID.find("S") == 0) return COLOR_SCHOOL;
//    if (vertexID.find("Mall") == 0 || vertexID.find("M") == 0) return COLOR_MALL;
//    if (vertexID.find("Pharmacy") == 0 || vertexID.find("P") == 0) return COLOR_PHARMACY;
//    if (vertexID.find("Facility") == 0 || vertexID.find("F") == 0) return COLOR_FACILITY;
//    if (vertexID.find("AIR") == 0 || vertexID.find("Airport") == 0 || vertexID.find("APT") == 0) return COLOR_AIRPORT;
//    if (vertexID.find("RLY") == 0 || vertexID.find("Station") == 0 || vertexID.find("RS") == 0) return COLOR_RAILWAY;
//    return COLOR_BUS_STOP; // Default
//}
//
//int main() {
//    cout << "========================================" << endl;
//    cout << "  Smart City Graph Visualizer (SFML)" << endl;
//    cout << "========================================" << endl;
//    cout << "\nLoading data..." << endl;
//    
//    // Initialize shared graph and data loader
//    Graph* cityGraph = new Graph(500, false);
//    DataLoader* loader = new DataLoader();
//    
//    // Load stops
//    CSVRow stopData[200];
//    int stopCount = 0;
//    if (loader->loadStops("../SmartCity_dataset/stops.csv", stopData, 200, stopCount)) {
//        cout << "[OK] Loaded " << stopCount << " stops" << endl;
//        
//        for (int i = 0; i < stopCount; i++) {
//            if (stopData[i].fieldCount >= 3) {
//                string stopID = stopData[i].fields[0];
//                string name = stopData[i].fields[1];
//                string coordStr = stopData[i].fields[2];
//                
//                double lat, lon;
//                if (parseCoordinates(coordStr, lat, lon)) {
//                    cityGraph->addVertex(stopID, name, lat, lon);
//                }
//            }
//        }
//    }
//    
//    // Build road network (connect stops)
//    cityGraph->addEdgeWithDistance("Stop1", "Stop2");
//    cityGraph->addEdgeWithDistance("Stop2", "Stop3");
//    cityGraph->addEdgeWithDistance("Stop3", "Stop4");
//    cityGraph->addEdgeWithDistance("Stop1", "Stop5");
//    cityGraph->addEdgeWithDistance("Stop5", "Stop6");
//    cityGraph->addEdgeWithDistance("Stop6", "Stop7");
//    cityGraph->addEdgeWithDistance("Stop7", "Stop8");
//    cityGraph->addEdgeWithDistance("Stop4", "Stop7");
//    cityGraph->addEdgeWithDistance("Stop2", "Stop5");
//    cityGraph->addEdgeWithDistance("Stop6", "Stop9");
//    cityGraph->addEdgeWithDistance("Stop4", "Stop10");
//    cityGraph->addEdgeWithDistance("Stop10", "Stop11");
//    cityGraph->addEdgeWithDistance("Stop1", "Stop9");
//    cout << "[OK] Built road network (" << cityGraph->getEdgeCount() << " edges)" << endl;
//    
//    // Initialize modules with shared graph
//    EducationSector* education = new EducationSector(50, 11);
//    MedicalSector* medical = new MedicalSector(50, 100, 500, 11);
//    TransportManager* transport = new TransportManager(cityGraph, 100);
//    MallManager* malls = new MallManager(cityGraph);
//    FacilityManager* facilities = new FacilityManager(cityGraph);
//    AirportManager* airports = new AirportManager(cityGraph);
//    RailwayManager* railways = new RailwayManager(cityGraph);
//    
//    // Set shared graph for modules that need it
//    education->setCityGraph(cityGraph);
//    medical->setCityGraph(cityGraph);
//    
//    // Load schools (they will be added to the shared graph automatically)
//    int schoolCount = education->loadSchoolsFromCSV("../SmartCity_dataset/schools.csv");
//    if (schoolCount > 0) {
//        cout << "[OK] Loaded " << schoolCount << " schools" << endl;
//    }
//    
//    // Load hospitals (they will be added to the shared graph automatically)
//    int hospitalCount = medical->loadHospitalsFromCSV("../SmartCity_dataset/hospitals.csv");
//    if (hospitalCount > 0) {
//        cout << "[OK] Loaded " << hospitalCount << " hospitals" << endl;
//    }
//    
//    // Load pharmacies (they will be added to the shared graph automatically)
//    int pharmacyCount = medical->loadPharmaciesFromCSV("../SmartCity_dataset/pharmacies.csv");
//    if (pharmacyCount > 0) {
//        cout << "[OK] Loaded " << pharmacyCount << " pharmacies" << endl;
//    }
//    
//    // Use the shared graph from modules (they all use the same graph)
//    Graph* sharedGraph = education->getCityGraph();
//    if (sharedGraph != nullptr) {
//        cityGraph = sharedGraph; // Use the shared graph instead
//        cout << "[OK] Using shared graph from modules" << endl;
//    }
//    
//    // Load malls
//    CSVRow mallData[100];
//    int mallCount = 0;
//    if (loader->loadMalls("../SmartCity_dataset/malls.csv", mallData, 100, mallCount)) {
//        malls->loadMallsFromCSVData(mallData, mallCount);
//        cout << "[OK] Loaded " << mallCount << " malls" << endl;
//    }
//    
//    // Load facilities
//    CSVRow facilityData[100];
//    int facilityCount = 0;
//    if (loader->loadFacilities("../SmartCity_dataset/facilities.csv", facilityData, 100, facilityCount)) {
//        facilities->loadFacilitiesFromCSVData(facilityData, facilityCount);
//        cout << "[OK] Loaded " << facilityCount << " facilities" << endl;
//    }
//    
//    // Load airports
//    CSVRow airportData[50];
//    int airportCount = 0;
//    if (loader->loadAirports("../SmartCity_dataset/airports.csv", airportData, 50, airportCount)) {
//        airports->loadAirportsFromCSVData(airportData, airportCount);
//        cout << "[OK] Loaded " << airportCount << " airports" << endl;
//    }
//    
//    // Load railways
//    CSVRow railwayData[50];
//    int railwayCount = 0;
//    if (loader->loadRailways("../SmartCity_dataset/railways.csv", railwayData, 50, railwayCount)) {
//        railways->loadStationsFromCSVData(railwayData, railwayCount);
//        cout << "[OK] Loaded " << railwayCount << " railway stations" << endl;
//    }
//    
//    // Find bounds for mapping
//    double minLat = 90.0, maxLat = -90.0;
//    double minLon = 180.0, maxLon = -180.0;
//    
//    for (int i = 0; i < cityGraph->getVertexCount(); i++) {
//        Vertex* v = cityGraph->getVertexAt(i);
//        if (v == nullptr) continue;
//        
//        if (v->latitude < minLat) minLat = v->latitude;
//        if (v->latitude > maxLat) maxLat = v->latitude;
//        if (v->longitude < minLon) minLon = v->longitude;
//        if (v->longitude > maxLon) maxLon = v->longitude;
//    }
//    
//    // Add some padding
//    double latRange = maxLat - minLat;
//    double lonRange = maxLon - minLon;
//    if (latRange > 0.001) {
//        minLat -= latRange * 0.1;
//        maxLat += latRange * 0.1;
//    }
//    if (lonRange > 0.001) {
//        minLon -= lonRange * 0.1;
//        maxLon += lonRange * 0.1;
//    }
//    
//    cout << "\nGraph bounds:" << endl;
//    cout << "Latitude: " << minLat << " to " << maxLat << endl;
//    cout << "Longitude: " << minLon << " to " << maxLon << endl;
//    cout << "\nVertices: " << cityGraph->getVertexCount() << endl;
//    cout << "Edges: " << cityGraph->getEdgeCount() << endl;
//    cout << "\nOpening visualization window..." << endl;
//    cout << "Controls:" << endl;
//    cout << "  - Mouse: Hover over vertices to see names" << endl;
//    cout << "  - Mouse Wheel: Zoom in/out" << endl;
//    cout << "  - Mouse Drag: Pan the view" << endl;
//    cout << "  - Click buttons on left side to toggle vertex types" << endl;
//    cout << "  - +/- Keys: Zoom in/out" << endl;
//    cout << "  - Arrow Keys: Pan the view" << endl;
//    cout << "  - R: Reset zoom and pan" << endl;
//    cout << "  - L: Toggle labels" << endl;
//    cout << "  - ESC: Close window" << endl;
//    
//    // Create SFML window
//    const int WINDOW_WIDTH = 1400;
//    const int WINDOW_HEIGHT = 900;
//    
//    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
//                           "Smart City Network Graph Visualization");
//    window.setFramerateLimit(60);
//    
//    // Load font (try default system font first, then fallback)
//    sf::Font font;
//    bool fontLoaded = false;
//    
//    // Try common system font paths
//    const char* fontPaths[] = {
//        "C:/Windows/Fonts/arial.ttf",
//        "C:/Windows/Fonts/calibri.ttf",
//        "C:/Windows/Fonts/tahoma.ttf",
//        "C:/Windows/Fonts/consola.ttf"
//    };
//    
//    for (int i = 0; i < 4; i++) {
//        if (font.loadFromFile(fontPaths[i])) {
//            fontLoaded = true;
//            cout << "[OK] Loaded font: " << fontPaths[i] << endl;
//            break;
//        }
//    }
//    
//    if (!fontLoaded) {
//        cout << "[WARNING] Could not load system font. Text may not render." << endl;
//        cout << "[INFO] Text will still be displayed in console." << endl;
//    }
//    
//    bool showLabels = false;
//    int hoveredVertex = -1;
//    string hoveredVertexName = "";
//    
//    // Vertex type visibility toggles
//    bool showBusStops = true;
//    bool showHospitals = true;
//    bool showSchools = true;
//    bool showMalls = true;
//    bool showPharmacies = true;
//    bool showFacilities = true;
//    bool showAirports = true;
//    bool showRailways = true;
//    
//    // Zoom and pan variables
//    float zoomLevel = 1.0f;
//    float panX = 0.0f;
//    float panY = 0.0f;
//    const float zoomSpeed = 0.1f;
//    const float panSpeed = 10.0f;
//    const float minZoom = 0.1f;
//    const float maxZoom = 5.0f;
//    
//    // Mouse drag for panning
//    bool isDragging = false;
//    sf::Vector2i lastMousePos;
//    
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed) {
//                window.close();
//            }
//            
//            if (event.type == sf::Event::KeyPressed) {
//                if (event.key.code == sf::Keyboard::Escape) {
//                    window.close();
//                }
//                if (event.key.code == sf::Keyboard::L) {
//                    showLabels = !showLabels;
//                }
//                // Vertex type visibility toggles
//                if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
//                    showBusStops = !showBusStops;
//                }
//                if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) {
//                    showHospitals = !showHospitals;
//                }
//                if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3) {
//                    showSchools = !showSchools;
//                }
//                if (event.key.code == sf::Keyboard::Num4 || event.key.code == sf::Keyboard::Numpad4) {
//                    showMalls = !showMalls;
//                }
//                if (event.key.code == sf::Keyboard::Num5 || event.key.code == sf::Keyboard::Numpad5) {
//                    showPharmacies = !showPharmacies;
//                }
//                if (event.key.code == sf::Keyboard::Num6 || event.key.code == sf::Keyboard::Numpad6) {
//                    showFacilities = !showFacilities;
//                }
//                if (event.key.code == sf::Keyboard::Num7 || event.key.code == sf::Keyboard::Numpad7) {
//                    showAirports = !showAirports;
//                }
//                if (event.key.code == sf::Keyboard::Num8 || event.key.code == sf::Keyboard::Numpad8) {
//                    showRailways = !showRailways;
//                }
//                // Show all / Hide all
//                if (event.key.code == sf::Keyboard::A) {
//                    bool allVisible = showBusStops && showHospitals && showSchools && showMalls && 
//                                     showPharmacies && showFacilities && showAirports && showRailways;
//                    showBusStops = showHospitals = showSchools = showMalls = 
//                    showPharmacies = showFacilities = showAirports = showRailways = !allVisible;
//                }
//                // Zoom controls
//                if (event.key.code == sf::Keyboard::Equal || event.key.code == sf::Keyboard::Add) {
//                    zoomLevel = min(zoomLevel + zoomSpeed, maxZoom);
//                }
//                if (event.key.code == sf::Keyboard::Hyphen || event.key.code == sf::Keyboard::Subtract) {
//                    zoomLevel = max(zoomLevel - zoomSpeed, minZoom);
//                }
//                // Reset zoom and pan
//                if (event.key.code == sf::Keyboard::R) {
//                    zoomLevel = 1.0f;
//                    panX = 0.0f;
//                    panY = 0.0f;
//                }
//                // Pan controls (arrow keys)
//                if (event.key.code == sf::Keyboard::Left) {
//                    panX += panSpeed;
//                }
//                if (event.key.code == sf::Keyboard::Right) {
//                    panX -= panSpeed;
//                }
//                if (event.key.code == sf::Keyboard::Up) {
//                    panY += panSpeed;
//                }
//                if (event.key.code == sf::Keyboard::Down) {
//                    panY -= panSpeed;
//                }
//            }
//            
//            // Mouse wheel zoom
//            if (event.type == sf::Event::MouseWheelScrolled) {
//                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
//                    float delta = event.mouseWheelScroll.delta;
//                    zoomLevel += delta * zoomSpeed;
//                    zoomLevel = max(minZoom, min(zoomLevel, maxZoom));
//                }
//            }
//            
//            // Mouse drag for panning (only if not clicking on buttons)
//            if (event.type == sf::Event::MouseButtonPressed) {
//                if (event.mouseButton.button == sf::Mouse::Left) {
//                    sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
//                    
//                    // Check if click is in colored square area
//                    float legendX = 20.0f;
//                    float legendY = 20.0f;
//                    float squareSpacing = 30.0f;
//                    float squareWidth = 140.0f;
//                    float squareHeight = 25.0f;
//                    float toggleAllY = legendY + 8 * squareSpacing + 10.0f;
//                    float toggleAllWidth = 140.0f;
//                    float toggleAllHeight = 25.0f;
//                    
//                    bool inButtonArea = false;
//                    // Check colored squares
//                    for (int i = 0; i < 8; i++) {
//                        float squareY = legendY + i * squareSpacing;
//                        if (mousePos.x >= legendX && mousePos.x <= legendX + squareWidth &&
//                            mousePos.y >= squareY && mousePos.y <= squareY + squareHeight) {
//                            inButtonArea = true;
//                            break;
//                        }
//                    }
//                    // Check toggle all square
//                    if (!inButtonArea && mousePos.x >= legendX && mousePos.x <= legendX + toggleAllWidth &&
//                        mousePos.y >= toggleAllY && mousePos.y <= toggleAllY + toggleAllHeight) {
//                        inButtonArea = true;
//                    }
//                    
//                    // Only start dragging if not clicking on buttons
//                    if (!inButtonArea) {
//                        isDragging = true;
//                        lastMousePos = mousePos;
//                    }
//                }
//            }
//            
//            if (event.type == sf::Event::MouseButtonReleased) {
//                if (event.mouseButton.button == sf::Mouse::Left) {
//                    isDragging = false;
//                }
//            }
//            
//            // Mouse move handling (both dragging and hover detection)
//            if (event.type == sf::Event::MouseMoved) {
//                if (isDragging) {
//                    // Handle panning while dragging
//                    sf::Vector2i currentMousePos(event.mouseMove.x, event.mouseMove.y);
//                    panX += (currentMousePos.x - lastMousePos.x);
//                    panY += (currentMousePos.y - lastMousePos.y);
//                    lastMousePos = currentMousePos;
//                } else {
//                    // Mouse hover detection (only when not dragging)
//                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
//                    hoveredVertex = -1;
//                    hoveredVertexName = "";
//                    
//                    for (int i = 0; i < cityGraph->getVertexCount(); i++) {
//                        Vertex* v = cityGraph->getVertexAt(i);
//                        if (v == nullptr) continue;
//                        
//                        sf::Vector2f screenPos = mapToScreen(v->latitude, v->longitude, 
//                                                              minLat, maxLat, minLon, maxLon,
//                                                              WINDOW_WIDTH, WINDOW_HEIGHT,
//                                                              zoomLevel, panX, panY);
//                        
//                        float dx = mousePos.x - screenPos.x;
//                        float dy = mousePos.y - screenPos.y;
//                        float distance = sqrt(dx * dx + dy * dy);
//                        
//                        // Adjust hover radius based on zoom
//                        float hoverRadius = 12.0f / zoomLevel;
//                        if (distance < hoverRadius) {
//                            hoveredVertex = i;
//                            hoveredVertexName = v->name + " (" + v->vertexID + ")";
//                            break;
//                        }
//                    }
//                }
//            }
//            
//            // Mouse click handling for buttons
//            if (event.type == sf::Event::MouseButtonPressed) {
//                if (event.mouseButton.button == sf::Mouse::Left && !isDragging) {
//                    sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
//                    
//                    // Check if click is on a colored square button
//                    float legendX = 20.0f;
//                    float legendY = 20.0f;
//                    float squareSpacing = 30.0f;
//                    float squareWidth = 140.0f;
//                    float squareHeight = 25.0f;
//                    
//                    for (int i = 0; i < 8; i++) {
//                        float squareY = legendY + i * squareSpacing;
//                        if (mousePos.x >= legendX && mousePos.x <= legendX + squareWidth &&
//                            mousePos.y >= squareY && mousePos.y <= squareY + squareHeight) {
//                            // Toggle corresponding vertex type
//                            switch (i) {
//                                case 0: showBusStops = !showBusStops; break;
//                                case 1: showHospitals = !showHospitals; break;
//                                case 2: showSchools = !showSchools; break;
//                                case 3: showMalls = !showMalls; break;
//                                case 4: showPharmacies = !showPharmacies; break;
//                                case 5: showFacilities = !showFacilities; break;
//                                case 6: showAirports = !showAirports; break;
//                                case 7: showRailways = !showRailways; break;
//                            }
//                            break;
//                        }
//                    }
//                    
//                    // Check if click is on "Toggle All" square
//                    float toggleAllY = legendY + 8 * squareSpacing + 10.0f;
//                    float toggleAllWidth = 140.0f;
//                    float toggleAllHeight = 25.0f;
//                    if (mousePos.x >= legendX && mousePos.x <= legendX + toggleAllWidth &&
//                        mousePos.y >= toggleAllY && mousePos.y <= toggleAllY + toggleAllHeight) {
//                        bool allVisible = showBusStops && showHospitals && showSchools && showMalls && 
//                                         showPharmacies && showFacilities && showAirports && showRailways;
//                        showBusStops = showHospitals = showSchools = showMalls = 
//                        showPharmacies = showFacilities = showAirports = showRailways = !allVisible;
//                    }
//                }
//            }
//        }
//        
//        // Clear window
//        window.clear(COLOR_BACKGROUND);
//        
//        // Helper function to check if vertex type should be visible
//        auto isVertexTypeVisible = [&](const string& vertexID) -> bool {
//            if (vertexID.find("Stop") == 0) return showBusStops;
//            if (vertexID.find("Hospital") == 0 || vertexID.find("H") == 0) return showHospitals;
//            if (vertexID.find("School") == 0 || vertexID.find("S") == 0) return showSchools;
//            if (vertexID.find("Mall") == 0 || vertexID.find("M") == 0) return showMalls;
//            if (vertexID.find("Pharmacy") == 0 || vertexID.find("P") == 0) return showPharmacies;
//            if (vertexID.find("Facility") == 0 || vertexID.find("F") == 0) return showFacilities;
//            if (vertexID.find("AIR") == 0 || vertexID.find("Airport") == 0 || vertexID.find("APT") == 0) return showAirports;
//            if (vertexID.find("RLY") == 0 || vertexID.find("Station") == 0 || vertexID.find("RS") == 0) return showRailways;
//            return showBusStops; // Default
//        };
//        
//        // Draw edges first (so they appear behind vertices)
//        // Only draw edges between visible vertices
//        for (int i = 0; i < cityGraph->getVertexCount(); i++) {
//            Vertex* v1 = cityGraph->getVertexAt(i);
//            if (v1 == nullptr || !isVertexTypeVisible(v1->vertexID)) continue;
//            
//            sf::Vector2f pos1 = mapToScreen(v1->latitude, v1->longitude, 
//                                           minLat, maxLat, minLon, maxLon,
//                                           WINDOW_WIDTH, WINDOW_HEIGHT,
//                                           zoomLevel, panX, panY);
//            
//            int destinations[100];
//            double weights[100];
//            int edgeCount = 0;
//            cityGraph->getEdgesForVertex(i, destinations, weights, 100, edgeCount);
//            
//            for (int j = 0; j < edgeCount; j++) {
//                int destIndex = destinations[j];
//                if (destIndex < 0 || destIndex >= cityGraph->getVertexCount()) continue;
//                
//                Vertex* v2 = cityGraph->getVertexAt(destIndex);
//                if (v2 == nullptr || !isVertexTypeVisible(v2->vertexID)) continue;
//                
//                // Only draw edge once (for undirected graph)
//                if (i < destIndex) {
//                    sf::Vector2f pos2 = mapToScreen(v2->latitude, v2->longitude, 
//                                                   minLat, maxLat, minLon, maxLon,
//                                                   WINDOW_WIDTH, WINDOW_HEIGHT,
//                                                   zoomLevel, panX, panY);
//                    
//                    // Draw edge line with thickness based on zoom
//                    // Use rectangles to simulate line thickness since SFML Lines don't support it
//                    float lineThickness = max(1.0f, 1.5f * zoomLevel);
//                    lineThickness = min(lineThickness, 4.0f); // Max thickness
//                    
//                    // Calculate line direction
//                    float dx = pos2.x - pos1.x;
//                    float dy = pos2.y - pos1.y;
//                    float length = sqrt(dx * dx + dy * dy);
//                    
//                    if (length > 0.1f) {
//                        // Normalize direction
//                        float nx = dx / length;
//                        float ny = dy / length;
//                        
//                        // Perpendicular vector for thickness
//                        float perpX = -ny * lineThickness * 0.5f;
//                        float perpY = nx * lineThickness * 0.5f;
//                        
//                        // Create rectangle shape for the line
//                        sf::ConvexShape lineRect;
//                        lineRect.setPointCount(4);
//                        lineRect.setPoint(0, sf::Vector2f(pos1.x + perpX, pos1.y + perpY));
//                        lineRect.setPoint(1, sf::Vector2f(pos1.x - perpX, pos1.y - perpY));
//                        lineRect.setPoint(2, sf::Vector2f(pos2.x - perpX, pos2.y - perpY));
//                        lineRect.setPoint(3, sf::Vector2f(pos2.x + perpX, pos2.y + perpY));
//                        lineRect.setFillColor(COLOR_EDGE);
//                        window.draw(lineRect);
//                    } else {
//                        // Fallback to simple line if length is too small
//                        sf::Vertex line[] = {
//                            sf::Vertex(pos1, COLOR_EDGE),
//                            sf::Vertex(pos2, COLOR_EDGE)
//                        };
//                        window.draw(line, 2, sf::Lines);
//                    }
//                }
//            }
//        }
//        
//        // Draw vertices (only visible types)
//        for (int i = 0; i < cityGraph->getVertexCount(); i++) {
//            Vertex* v = cityGraph->getVertexAt(i);
//            if (v == nullptr || !isVertexTypeVisible(v->vertexID)) continue;
//            
//            sf::Vector2f screenPos = mapToScreen(v->latitude, v->longitude, 
//                                                 minLat, maxLat, minLon, maxLon,
//                                                 WINDOW_WIDTH, WINDOW_HEIGHT,
//                                                 zoomLevel, panX, panY);
//            
//            sf::Color vertexColor = getVertexColor(v->vertexID);
//            
//            // Highlight hovered vertex
//            if (i == hoveredVertex) {
//                vertexColor = COLOR_HOVER;
//            }
//            
//            // Draw vertex circle (scale with zoom)
//            float baseRadius = (i == hoveredVertex) ? 8.0f : 6.0f;
//            float radius = baseRadius * zoomLevel;
//            // Allow more scaling range for better zoom experience
//            radius = max(1.0f, min(radius, 30.0f));
//            sf::CircleShape vertexCircle(radius);
//            vertexCircle.setPosition(screenPos.x - radius, screenPos.y - radius);
//            vertexCircle.setFillColor(vertexColor);
//            vertexCircle.setOutlineColor(sf::Color::White);
//            // Scale outline thickness with zoom
//            float outlineThickness = max(0.5f, 1.5f * zoomLevel);
//            vertexCircle.setOutlineThickness(outlineThickness);
//            window.draw(vertexCircle);
//            
//            // Draw label if enabled or hovered
//            if (fontLoaded && (showLabels || i == hoveredVertex)) {
//                sf::Text label;
//                label.setFont(font);
//                label.setString(v->name);
//                label.setCharacterSize((unsigned int)(12 * zoomLevel));
//                label.setFillColor(sf::Color::White);
//                label.setOutlineColor(sf::Color::Black);
//                label.setOutlineThickness(1.0f);
//                label.setPosition(screenPos.x + 10.0f * zoomLevel, screenPos.y - 8.0f * zoomLevel);
//                window.draw(label);
//            }
//        }
//        
//        // Draw legend
//        float legendX = 20.0f;
//        float legendY = 20.0f;
//        float legendSpacing = 25.0f;
//        
//        // Legend items
//        struct LegendItem {
//            sf::Color color;
//            string label;
//        };
//        
//        LegendItem legend[] = {
//            {COLOR_BUS_STOP, "Bus Stop"},
//            {COLOR_HOSPITAL, "Hospital"},
//            {COLOR_SCHOOL, "School"},
//            {COLOR_MALL, "Mall"},
//            {COLOR_PHARMACY, "Pharmacy"},
//            {COLOR_FACILITY, "Facility"},
//            {COLOR_AIRPORT, "Airport"},
//            {COLOR_RAILWAY, "Railway Station"}
//        };
//        
//        // Draw clickable colored squares with text labels (legend/buttons)
//        bool visibilityFlags[] = {
//            showBusStops, showHospitals, showSchools, showMalls,
//            showPharmacies, showFacilities, showAirports, showRailways
//        };
//        
//        // Get mouse position for button hover effect
//        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
//        
//        float squareWidth = 140.0f;
//        float squareHeight = 25.0f;
//        float squareSpacing = 30.0f;
//        
//        for (int i = 0; i < 8; i++) {
//            float squareY = legendY + i * squareSpacing;
//            
//            // Check if mouse is hovering over square
//            bool isHovered = (mousePos.x >= legendX && mousePos.x <= legendX + squareWidth &&
//                             mousePos.y >= squareY && mousePos.y <= squareY + squareHeight);
//            
//            // Draw colored square (slightly larger when hovered)
//            float currentWidth = isHovered ? squareWidth + 2.0f : squareWidth;
//            float currentHeight = isHovered ? squareHeight + 2.0f : squareHeight;
//            float offset = isHovered ? -1.0f : 0.0f;
//            
//            sf::RectangleShape colorSquare(sf::Vector2f(currentWidth, currentHeight));
//            colorSquare.setPosition(legendX + offset, squareY + offset);
//            colorSquare.setFillColor(legend[i].color);
//            
//            // Change outline based on visibility
//            if (visibilityFlags[i]) {
//                colorSquare.setOutlineColor(sf::Color::White);
//                colorSquare.setOutlineThickness(2.0f);
//            } else {
//                colorSquare.setOutlineColor(sf::Color(80, 80, 80)); // Dark gray when hidden
//                colorSquare.setOutlineThickness(2.0f);
//                // Make it semi-transparent when hidden
//                sf::Color hiddenColor = legend[i].color;
//                hiddenColor.a = 100; // Semi-transparent
//                colorSquare.setFillColor(hiddenColor);
//            }
//            
//            window.draw(colorSquare);
//            
//            // Draw text label inside square
//            if (fontLoaded) {
//                sf::Text labelText;
//                labelText.setFont(font);
//                labelText.setString("[" + legend[i].label + "]");
//                labelText.setCharacterSize(12);
//                labelText.setFillColor(sf::Color::White);
//                labelText.setOutlineColor(sf::Color::Black);
//                labelText.setOutlineThickness(1.0f);
//                
//                // Center text in square
//                sf::FloatRect textBounds = labelText.getLocalBounds();
//                float textX = legendX + (squareWidth - textBounds.width) / 2.0f + offset;
//                float textY = squareY + (squareHeight - textBounds.height) / 2.0f - 2.0f + offset;
//                labelText.setPosition(textX, textY);
//                
//                window.draw(labelText);
//            }
//        }
//        
//        // Draw "Toggle All" button with text
//        float toggleAllY = legendY + 8 * squareSpacing + 10.0f;
//        float toggleAllWidth = 140.0f;
//        float toggleAllHeight = 25.0f;
//        bool allVisible = showBusStops && showHospitals && showSchools && showMalls && 
//                         showPharmacies && showFacilities && showAirports && showRailways;
//        bool toggleAllHovered = (mousePos.x >= legendX && mousePos.x <= legendX + toggleAllWidth &&
//                                mousePos.y >= toggleAllY && mousePos.y <= toggleAllY + toggleAllHeight);
//        
//        float toggleCurrentWidth = toggleAllHovered ? toggleAllWidth + 2.0f : toggleAllWidth;
//        float toggleCurrentHeight = toggleAllHovered ? toggleAllHeight + 2.0f : toggleAllHeight;
//        float toggleOffset = toggleAllHovered ? -1.0f : 0.0f;
//        
//        sf::RectangleShape toggleAllSquare(sf::Vector2f(toggleCurrentWidth, toggleCurrentHeight));
//        toggleAllSquare.setPosition(legendX + toggleOffset, toggleAllY + toggleOffset);
//        
//        // Create a multi-color effect or use a special color
//        if (allVisible) {
//            toggleAllSquare.setFillColor(sf::Color(200, 200, 200)); // Light gray when all visible
//        } else {
//            toggleAllSquare.setFillColor(sf::Color(100, 100, 100)); // Dark gray when some hidden
//        }
//        toggleAllSquare.setOutlineColor(sf::Color::White);
//        toggleAllSquare.setOutlineThickness(2.0f);
//        window.draw(toggleAllSquare);
//        
//        // Draw "Toggle All" text
//        if (fontLoaded) {
//            sf::Text toggleText;
//            toggleText.setFont(font);
//            toggleText.setString("[Toggle All]");
//            toggleText.setCharacterSize(12);
//            toggleText.setFillColor(sf::Color::White);
//            toggleText.setOutlineColor(sf::Color::Black);
//            toggleText.setOutlineThickness(1.0f);
//            
//            // Center text in square
//            sf::FloatRect textBounds = toggleText.getLocalBounds();
//            float textX = legendX + (toggleAllWidth - textBounds.width) / 2.0f + toggleOffset;
//            float textY = toggleAllY + (toggleAllHeight - textBounds.height) / 2.0f - 2.0f + toggleOffset;
//            toggleText.setPosition(textX, textY);
//            
//            window.draw(toggleText);
//        }
//        
//        // Draw hovered vertex info
//        if (hoveredVertex >= 0 && !hoveredVertexName.empty()) {
//            // Draw info box background
//            sf::RectangleShape infoBox(sf::Vector2f(400.0f, 50.0f));
//            infoBox.setPosition(20.0f, WINDOW_HEIGHT - 70.0f);
//            infoBox.setFillColor(sf::Color(0, 0, 0, 220)); // Semi-transparent black
//            infoBox.setOutlineColor(sf::Color::White);
//            infoBox.setOutlineThickness(2.0f);
//            window.draw(infoBox);
//            
//            // Draw hovered vertex text
//            if (fontLoaded) {
//                sf::Text infoText;
//                infoText.setFont(font);
//                infoText.setString("Hovering: " + hoveredVertexName);
//                infoText.setCharacterSize(16);
//                infoText.setFillColor(sf::Color::Yellow);
//                infoText.setPosition(30.0f, WINDOW_HEIGHT - 60.0f);
//                window.draw(infoText);
//            }
//        }
//        
//        // Draw stats
//        if (fontLoaded) {
//            string statsText = "Vertices: " + to_string(cityGraph->getVertexCount()) + 
//                             " | Edges: " + to_string(cityGraph->getEdgeCount());
//            
//            sf::Text statsDisplay;
//            statsDisplay.setFont(font);
//            statsDisplay.setString(statsText);
//            statsDisplay.setCharacterSize(14);
//            statsDisplay.setFillColor(sf::Color::White);
//            statsDisplay.setPosition(20.0f, WINDOW_HEIGHT - 30.0f);
//            window.draw(statsDisplay);
//            
//            // Draw controls hint
//            string controlsText = showLabels ? "L: Hide labels | +/-: Zoom | Arrows: Pan | Mouse Drag: Pan | R: Reset | Click buttons to toggle types | ESC: Exit" : 
//                                               "L: Show labels | +/-: Zoom | Arrows: Pan | Mouse Drag: Pan | R: Reset | Click buttons to toggle types | ESC: Exit";
//            sf::Text controlsDisplay;
//            controlsDisplay.setFont(font);
//            controlsDisplay.setString(controlsText);
//            controlsDisplay.setCharacterSize(12);
//            controlsDisplay.setFillColor(sf::Color(200, 200, 200));
//            controlsDisplay.setPosition(20.0f, WINDOW_HEIGHT - 15.0f);
//            window.draw(controlsDisplay);
//            
//            // Draw zoom level
//            string zoomText = "Zoom: " + to_string((int)(zoomLevel * 100)) + "%";
//            sf::Text zoomDisplay;
//            zoomDisplay.setFont(font);
//            zoomDisplay.setString(zoomText);
//            zoomDisplay.setCharacterSize(12);
//            zoomDisplay.setFillColor(sf::Color(150, 255, 150));
//            zoomDisplay.setPosition(WINDOW_WIDTH - 150.0f, 20.0f);
//            window.draw(zoomDisplay);
//        }
//        
//        window.display();
//    }
//    
//    // Cleanup
//    delete railways;
//    delete airports;
//    delete facilities;
//    delete malls;
//    delete transport;
//    delete medical;
//    delete education;
//    delete cityGraph;
//    delete loader;
//    
//    cout << "\nVisualization closed." << endl;
//    return 0;
//}
//
