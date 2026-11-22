#pragma once
#include <Arduino.h>
#include <esp_http_server.h>
#include <functional>

// Adaptateur limite aux fonctions utilisees par ce programme.
class ServeurHttp {
  struct Route { ServeurHttp* serveur; std::function<void()> action; };
  httpd_handle_t handle = nullptr;
  httpd_req_t* req = nullptr;
  Route routes[32];
  unsigned int nbRoutes = 0;
  String corps, cookie, agent, host, headerName, headerValue;
  static int hex(char c) { if (c >= '0' && c <= '9') return c-'0'; if (c >= 'A' && c <= 'F') return c-'A'+10; if (c >= 'a' && c <= 'f') return c-'a'+10; return -1; }
  static String decode(const String& v) {
    String out;
    for (unsigned int i=0; i<v.length(); ++i) {
      if (v[i]=='+') out+=' ';
      else if (v[i]=='%' && i+2<v.length() && hex(v[i+1])>=0 && hex(v[i+2])>=0) { out+=(char)((hex(v[i+1])<<4)|hex(v[i+2])); i+=2; }
      else out+=v[i];
    }
    return out;
  }
  String lireHeader(const char* nom) {
    size_t n=httpd_req_get_hdr_value_len(req,nom);
    if (!n || n>2048) return String();
    char* buf=(char*)malloc(n+1); if (!buf) return String();
    String result; if (httpd_req_get_hdr_value_str(req,nom,buf,n+1)==ESP_OK) result=buf;
    free(buf); return result;
  }
  static esp_err_t traiter(httpd_req_t* r) {
    Route* route=static_cast<Route*>(r->user_ctx); ServeurHttp* self=route->serveur;
    self->req=r; self->corps=""; self->headerName=""; self->headerValue="";
    self->cookie=self->lireHeader("Cookie"); self->agent=self->lireHeader("User-Agent"); self->host=self->lireHeader("Host");
    if (r->content_len>4096) { httpd_resp_send_err(r,HTTPD_400_BAD_REQUEST,"Requete trop longue"); self->req=nullptr; return ESP_OK; }
    int reste=r->content_len; char buf[256];
    while (reste>0) {
      int n=httpd_req_recv(r,buf, (reste<(int)sizeof(buf)-1) ? reste : (int)sizeof(buf)-1);
      if (n<=0) { self->req=nullptr; return ESP_FAIL; }
      buf[n]=0; self->corps+=buf; reste-=n;
    }
    route->action(); self->req=nullptr; return ESP_OK;
  }
public:
  explicit ServeurHttp(int) {}
  void collectHeaders(const char**, size_t) {}
  bool estPost() { return req && req->method == HTTP_POST; }
  void on(const char* uri, httpd_method_t method, std::function<void()> fn) {
    if (!handle || nbRoutes>=32) return;
    Route* r=&routes[nbRoutes++]; r->serveur=this; r->action=fn;
    httpd_uri_t u={}; u.uri=uri; u.method=method; u.handler=traiter; u.user_ctx=r;
    if (httpd_register_uri_handler(handle,&u)!=ESP_OK) Serial.println("Erreur route HTTP");
  }
  void begin() {
    httpd_config_t c=HTTPD_DEFAULT_CONFIG(); c.max_uri_handlers=32; c.max_open_sockets=7; c.lru_purge_enable=true; c.stack_size=8192;
    if (httpd_start(&handle,&c)!=ESP_OK) Serial.println("Erreur demarrage serveur HTTP");
  }
  String header(const char* name) { if (!strcmp(name,"Cookie")) return cookie; if (!strcmp(name,"User-Agent")) return agent; if (!strcmp(name,"Host")) return host; return String(); }
  String arg(const char* name) {
    unsigned int pos=0;
    while (pos<corps.length()) {
      int end=corps.indexOf('&',pos); if (end<0) end=corps.length();
      int eq=corps.indexOf('=',pos); if (eq<0 || eq>end) eq=end;
      if (decode(corps.substring(pos,eq))==name) return decode(corps.substring(eq<end ? eq+1 : end,end));
      pos=end+1;
    }
    return String();
  }
  void sendHeader(const char* name,const String& value) { headerName=name; headerValue=value; }
  void send(int code,const char* type,const String& body) {
    if (!req) return;
    const char* status=code==200?"200 OK":code==400?"400 Bad Request":code==401?"401 Unauthorized":code==403?"403 Forbidden":code==404?"404 Not Found":code==429?"429 Too Many Requests":code==503?"503 Service Unavailable":"500 Internal Server Error";
    httpd_resp_set_status(req,status); httpd_resp_set_type(req,type);
    if (headerName.length()) httpd_resp_set_hdr(req,headerName.c_str(),headerValue.c_str());
    httpd_resp_send(req,body.c_str(),body.length());
  }
  void redirect(const char* url) {
    if (!req) return;
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", url);
    httpd_resp_send(req, "", 0);
  }
  void sendBytes(const char* type, const uint8_t* body, size_t size) {
    if (!req) return;
    httpd_resp_set_type(req,type);
    httpd_resp_set_hdr(req,"Cache-Control","public, max-age=86400");
    httpd_resp_send(req,reinterpret_cast<const char*>(body),size);
  }
  void sendGzip(const char* type, const uint8_t* body, size_t size) {
    if (!req) return;
    String accept = lireHeader("Accept-Encoding");
    if (accept.indexOf("gzip") < 0) {
      httpd_resp_set_status(req,"406 Not Acceptable");
      httpd_resp_sendstr(req,"Ce navigateur doit accepter gzip.");
      return;
    }
    httpd_resp_set_type(req,type);
    httpd_resp_set_hdr(req,"Content-Encoding","gzip");
    httpd_resp_set_hdr(req,"Vary","Accept-Encoding");
    httpd_resp_send(req, reinterpret_cast<const char*>(body), size);
  }
  void send_P(int code,const char* type,const char* body) {
    if (!req) return;
    httpd_resp_set_status(req,code==200?"200 OK":"500 Internal Server Error"); httpd_resp_set_type(req,type);
    httpd_resp_send(req,body,strlen(body));
  }
};
