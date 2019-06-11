/* 
 * File:   Server.cpp
 * 
 * Copyright (C) 2019 sorkinmp@miamioh.edu
 */

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "Server.h"

// The default file to return for "/"
const std::string RootFile = "index.html";

// A base string to be used in reading lines from a file
std::string lines;

// A message for the default file
std::string msg = "HTTP/1.1 200 OK\r\n" 
                      "Server: SimpleServer\r\n" 
                      "Content-Length: 355\r\n"
                      "Connection: Close\r\n" 
                      "Content-Type: text/html\r\n\r\n";

Server::Server() {
    // Nothing to be done in the constructor (for now).
}

Server::~Server() {
    // Nothing to be done in the destructor.
}


 /**
  * Serves one connection from 1 client by processing HTTP request
  * (ignoring headers) and responding to the request with contents 
  * of a file (specified in the GET request).
  * 
  * GET / HTTP/1.1 (for / file)
  * HTTP/1.1 200 OK
  * GET /testx.txt HTTP/1.1 (for error message)
  * 
  * @param is The input stream from where the client request is to be read.
  * @param os The output stream where the response is to be written.
  */
void Server::serveClient(std::istream& is, std::ostream& os) {
    std::string line;  // First extract request line from client
    std::getline(is, line);
    const size_t spc1Pos = line.find(' ');  // extract the file name
    const size_t spc2Pos = line.find(' ', spc1Pos + 1);
    std::string filePath = line.substr(spc1Pos + 1, spc2Pos - spc1Pos - 1);
    std::ifstream slashFile(RootFile);  // create stream for index.html
    if (filePath == "/") {
        os << msg;
        while (std::getline(slashFile, lines)) {
            os << lines << std::endl;
        }
    } else {
    filePath = filePath.substr(filePath.find("/") + 1);  // remove the "/"
    std::ifstream inFile(filePath);  // create file stream to process file
    inFile.seekg(0, std::ios::end);  
    const int fileSize = inFile.tellg();
    inFile.seekg(0);  // move file position back to beginning
    if (!inFile.good()) {  // if the file is not good
        errorMessage(inFile, os, filePath); 
    } else {
    allInfo(inFile, os, filePath, fileSize);  //  Send mime info to client
        while (std::getline(inFile, lines)) {  // while there are lines to read
            os << lines << std::endl; 
        }
      }  
    }  // send file lines to client
}

/**
 * Runs the program as a server processing incoming connections/requests
 * forever.
 * 
 * This method (along with helper methods) performs the following tasks
 *      1. Creates a server port and prints the port it is listening on.
 *      2. Accepts connections from clients and processes requests
 *         by calling the serveClient() method in this class.
 */
void Server::runServer() {
    // Setup a server socket to accept connections on the socket
    using namespace boost::asio;
    using namespace boost::asio::ip;
    io_service service;
    // Create end point. 0 == Pick any free port.
    tcp::endpoint myEndpoint(tcp::v4(), 0);
    // Create a socket that accepts connections
    tcp::acceptor server(service, myEndpoint);
    std::cout << "Listening on port " << server.local_endpoint().port()
              << std::endl;
    // Process client connections one-by-one...forever
    while (true) {
        tcp::iostream client;
        // Wait for a client to connect
        server.accept(*client.rdbuf());
        // Process information from client.
        serveClient(client, client);
    }
}

/**
 * This method will print out an appropriate error message based on the
 * file sent to it as a parameter.
 * 
 * @param inFile The file stream containing the file
 * @param os The output stream to deliver the error messsage
 * @param fileName The name of the file
 */
void Server::errorMessage(std::ifstream& inFile, std::ostream& os, 
        std::string fileName) {
    int fileSize = 35 + fileName.length();
    os << "HTTP/1.1 404 Not Found\r\n"
       << "Server: SimpleServer\r\n"
       << "Content-Length: " << std::to_string(fileSize) << "\r\n"
       << "Connection: Close\r\n"
       << "Content-Type: text/plain\r\n\r\n"
       << "The following file was not found: " << fileName << std::endl;
}

/**
 * This method will print out the http headers and MIME info that the
 * client needs to know, such as content type and content length
 * 
 * @param inFile The file stream containing the file
 * @param os The output stream to deliver the information
 * @param file The name of the file
 */
void Server::allInfo(std::ifstream& inFile, std::ostream& os,
        std::string fileName, const int fileSize) {
    std::string msg = "HTTP/1.1 200 OK\r\n" 
                      "Server: SimpleServer\r\n" 
                      "Content-Length: " + std::to_string(fileSize) + "\r\n"
                      "Connection: Close\r\n" 
                      "Content-Type: ";
    if (fileName.find(".html") != std::string::npos) {  // if file is .html
        os << msg << "text/html\r\n\r\n";
    } else if (fileName.find(".png") != std::string::npos) {  // file is .png
        os << msg << "image/png\r\n\r\n";
    } else if (fileName.find(".jpg") != std::string::npos) {  // file is .jpg
        os << msg << "image/jpeg\r\n\r\n";
    } else if (fileName.find(".txt") != std::string::npos) {  // file is .txt
        os << msg << "text/plain\r\n\r\n";
    } else {
    os << msg << "text/plain\r\n\r\n";  // default case
    }
}
