bool chacktopic(){
    String value = getEEprom("Mqtt");
    if(value.length() > 0 && value != ""){
        return true;
    }
    return false;
}

void setupMqtt() {
    if(!chacktopic()){
        return ;
    }
    // if(Client)

}

