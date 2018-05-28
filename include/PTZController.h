/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PTZController.h
 * Author: ha
 *
 * Created on August 23, 2017, 11:14 AM
 */

#ifndef PTZCONTROLLER_H
#define PTZCONTROLLER_H

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <sstream>

using namespace std;

class PTZController{
private:
    string cameraToken = "MainStream";
    string camUrl = "";
    static size_t CallbackFunc(void *contents, size_t size, size_t nmemb, std::string *s)
    {
        size_t newLength = size*nmemb;
        size_t oldLength = s->size();
        try
        {
            s->resize(oldLength + newLength);
        }
        catch(std::bad_alloc &e)
        {
            //handle memory problem
            return 0;
        }

        std::copy((char*)contents,(char*)contents+newLength,s->begin()+oldLength);
        return size*nmemb;
    }
public:    
    PTZController(string ip)
    {camUrl = "http://" + ip + "/onvif/device_service";}
    
    string post(string content){
        std::string respondContent = "";
        CURL*curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, camUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CallbackFunc);


            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respondContent);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();

        return respondContent;
    }

    string getToken(){
        std::string respondContent = "";
        CURL*curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, camUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CallbackFunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respondContent);
            string strGetProfile = "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://www.w3.org/2005/08/addressing\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">admin</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\"></Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"></Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><GetProfiles xmlns=\"http://www.onvif.org/ver10/media/wsdl\"/></s:Body> </s:Envelope>";
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strGetProfile.c_str());
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            //std::cout<<"Returned:"<<respondContent;
        }
        return "";
    }
    
    enum Dir{
        Left,Right,Up,Down
    };
    void stop(){
        post("<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://www.w3.org/2005/08/addressing\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken> <Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">admin</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">MainStream</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"></Created></UsernameToken> </Security></s:Header> <s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><Stop xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\"><ProfileToken>"+cameraToken+"</ProfileToken><PanTilt>true</PanTilt><Zoom>true</Zoom></Stop></s:Body> </s:Envelope>");
    }
    void move(Dir dir, int miliSecond){
       string dirCmd;
       switch(dir){
           case Left:
               dirCmd = "<PanTilt x=\"-1\" y=\"0\" xmlns=\"http://www.onvif.org/ver10/schema\" />";
               break;
           case Right:
               dirCmd = "<PanTilt x=\"1\" y=\"0\" xmlns=\"http://www.onvif.org/ver10/schema\" />";
               break;
           case Up:
               dirCmd = "<PanTilt x=\"0\" y=\"1\" xmlns=\"http://www.onvif.org/ver10/schema\" />";
               break;
           case Down:
               dirCmd = "<PanTilt x=\"0\" y=\"-1\" xmlns=\"http://www.onvif.org/ver10/schema\" />";
               break;
       }
       string cmd = "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://www.w3.org/2005/08/addressing\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">admin</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">" + cameraToken + "</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"></Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><ContinuousMove xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\"><ProfileToken>" + cameraToken +"</ProfileToken><Velocity> " + dirCmd + "<Zoom x=\"0\" xmlns=\"http://www.onvif.org/ver10/schema\" /></Velocity><Timeout>0</Timeout></ContinuousMove></s:Body></s:Envelope>";
       if(post(cmd) != ""){
           std::this_thread::sleep_for(std::chrono::milliseconds(miliSecond));
           stop();
       }
    }


    void move(float x, float y){
       
       
        string dirCmd = "<PanTilt x=\""+ MyFloatToString(x) + "\" y=\""+ MyFloatToString(y) +"\" xmlns=\"http://www.onvif.org/ver10/schema\" />";
            
        string cmd = "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://www.w3.org/2005/08/addressing\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">admin</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">" + cameraToken + "</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"></Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><ContinuousMove xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\"><ProfileToken>" + cameraToken +"</ProfileToken><Velocity> " + dirCmd + "<Zoom x=\"0\" xmlns=\"http://www.onvif.org/ver10/schema\" /></Velocity><Timeout>0</Timeout></ContinuousMove></s:Body></s:Envelope>";
       post(cmd);
    }


    void move(float x, float y, int miliSecond){
        string dirCmd;
        if(x>0)
            dirCmd = "<PanTilt x=\"1\" y=\"0\" xmlns=\"http://www.onvif.org/ver10/schema\" />";
        else if(x<0)
            dirCmd = "<PanTilt x=\"-1\" y=\"0\" xmlns=\"http://www.onvif.org/ver10/schema\" />";
        else if(y>0)
            dirCmd = "<PanTilt x=\"0\" y=\"1\" xmlns=\"http://www.onvif.org/ver10/schema\" />";
        else if(y<0)
            dirCmd = "<PanTilt x=\"0\" y=\"-1\" xmlns=\"http://www.onvif.org/ver10/schema\" />";
        string cmd = "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://www.w3.org/2005/08/addressing\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">admin</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">" + cameraToken + "</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"></Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><ContinuousMove xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\"><ProfileToken>" + cameraToken +"</ProfileToken><Velocity> " + dirCmd + "<Zoom x=\"0\" xmlns=\"http://www.onvif.org/ver10/schema\" /></Velocity><Timeout>0</Timeout></ContinuousMove></s:Body></s:Envelope>";
        if(post(cmd) != ""){
            std::this_thread::sleep_for(std::chrono::milliseconds(miliSecond));
            stop();
        }
    }

    void moveDir(float x, float y, int miliSecond){
        string dirCmd;
        //if(x>0)
        dirCmd = "<PanTilt x=\"" + MyFloatToString(x) + "\" y=\"" + MyFloatToString(y) + "\" xmlns=\"http://www.onvif.org/ver10/schema\" />";
        /*else if(x<0)
            dirCmd = "<PanTilt x=\"-1\" y=\"0\" xmlns=\"http://www.onvif.org/ver10/schema\" />";
        else if(y>0)
            dirCmd = "<PanTilt x=\"0\" y=\"1\" xmlns=\"http://www.onvif.org/ver10/schema\" />";
        else if(y<0)
            dirCmd = "<PanTilt x=\"0\" y=\"-1\" xmlns=\"http://www.onvif.org/ver10/schema\" />";*/
        string cmd = "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://www.w3.org/2005/08/addressing\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">admin</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">" + cameraToken + "</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"></Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><ContinuousMove xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\"><ProfileToken>" + cameraToken +"</ProfileToken><Velocity> " + dirCmd + "<Zoom x=\"0\" xmlns=\"http://www.onvif.org/ver10/schema\" /></Velocity><Timeout>0</Timeout></ContinuousMove></s:Body></s:Envelope>";
        if(post(cmd) != ""){
            std::this_thread::sleep_for(std::chrono::milliseconds(miliSecond));
            stop();
        }
    }
    std::string MyIntToString ( int Number )
    {
      std::ostringstream ss;
      ss << Number;
      return ss.str();
    }

    std::string MyFloatToString ( float Number )
    {
      std::ostringstream ss;
      ss << Number;
      return ss.str();
    }
    void absoluteMove(float x, float y){
        string dirCmd = "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://www.w3.org/2005/08/addressing\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">admin</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">" + cameraToken + "</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"></Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                            +"<AbsoluteMove xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\">"
                            +"<ProfileToken>" + cameraToken + "</ProfileToken>"
                            +"<Position> <PanTilt x=\"" + MyFloatToString(x) + "\" y=\"" + MyFloatToString(y) + "\" xmlns=\"http://www.onvif.org/ver10/schema\" /></Position>"
                            +"<Speed> <PanTilt x=\"1\" y=\"1\" xmlns=\"http://www.onvif.org/ver10/schema\" />"
                            +"</Speed>"
                        + "</AbsoluteMove>"
                        + "</s:Body>"
                        +"</s:Envelope>";
        post(dirCmd);
    }
    void absoluteMove(float x, float y, float valueZoom){
        string dirCmd = "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://www.w3.org/2005/08/addressing\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">admin</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">" + cameraToken + "</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"></Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                            +"<AbsoluteMove xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\">"
                            +"<ProfileToken>" + cameraToken + "</ProfileToken>"
                            +"<Position>"
                            +"<PanTilt x=\"" + MyFloatToString(x) + "\" y=\"" + MyFloatToString(y) + "\" xmlns=\"http://www.onvif.org/ver10/schema\" />"
                            +"<Zoom x=\"" + MyFloatToString(valueZoom) + "\" xmlns=\"http://www.onvif.org/ver10/schema\" /></Position>"
                            +"<Speed> "
                            +"<PanTilt x=\"1\" y=\"1\" xmlns=\"http://www.onvif.org/ver10/schema\" />"
                            +"<Zoom x=\"1\" xmlns=\"http://www.onvif.org/ver10/schema\"/>"
                            +"</Speed>"
                        + "</AbsoluteMove>"
                        + "</s:Body>"
                        +"</s:Envelope>";
        post(dirCmd);
    }

    void relativeMove(float x, float y){
        string dirCmd = "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://www.w3.org/2005/08/addressing\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">admin</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">" + cameraToken + "</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"></Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                            +"<RelativeMove xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\">"
                            +"<ProfileToken>" + cameraToken + "</ProfileToken>"
                            +"<Translation>"
                            +"<PanTilt x=\"" + MyFloatToString(x) + "\" y=\"" + MyFloatToString(y) + "\" xmlns=\"http://www.onvif.org/ver10/schema\" />"
                            +"<Zoom x=\"0\" xmlns=\"http://www.onvif.org/ver10/schema\" /></Position>"
                            +"<Speed> "
                            +"<PanTilt x=\"0.1\" y=\"0.1\" xmlns=\"http://www.onvif.org/ver10/schema\" />"
                            +"<Zoom x=\"0\" xmlns=\"http://www.onvif.org/ver10/schema\"/>"
                            +"</Speed>"
                        + "</RelativeMove>"
                        + "</s:Body>"
                        +"</s:Envelope>";
        post(dirCmd);
        stop();
    }


    void absoluteMoveAngle(float x, float y){
        //post(dirCmd);
    }
    
    void moveLeft(int miliSecond){  move(Dir::Left, miliSecond); }
    void moveRight(int miliSecond){  move(Dir::Right, miliSecond); }
    void moveUp(int miliSecond){  move(Dir::Up, miliSecond); }
    void moveDown(int miliSecond){  move(Dir::Down, miliSecond); }
};


#endif /* PTZCONTROLLER_H */

