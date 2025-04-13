/*
This example covers a basic pub sub model. some design decisions will be improved upon
Single event bus is declared
Passed as a reference to publisher and subsribers
Publisher writes events to topics
Sub can subscribe and unsub to a topic
Event bus -> handles calls callbacks when topic is written to

Things to look for in this example:
1. Forward dec;aration of Sub class
2. Forward declaration of EventBus::publish_event
3. object instances passing themselves as references

TODO:
1. topics should maintain callback func pointers rather than sub objects itself to support different cbs for dif topics
2. Use Eventbus as singleton
3. Remove circular dependency with EventBus and Sub
*/

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// Forward declaration for Sub
class Sub;

class EventBus
{
public:
    unordered_set<string> topics;
    unordered_map<string, unordered_set<Sub *>> listeners;
    unordered_map<string, vector<string>> events;

    EventBus()
    {
        topics = {"t1", "t2", "t3"};
    }

    void subscribe_topic(const string &topicId, Sub *sub)
    {
        listeners[topicId].insert(sub);
    }

    void unsubscribe_topic(const string &topicId, Sub *sub)
    {
        listeners[topicId].erase(sub);
    }

    void push_event(const string &topicId, const string &event);
};

class Pub
{
public:
    string id;
    EventBus *eventBus;

    Pub(string id, EventBus *eventBus) : id(id), eventBus(eventBus) {}

    void addEvent(const string &topic, const string &event)
    {
        eventBus->push_event(topic, event);
    }
};

class Sub
{
public:
    string id;
    EventBus *eventBus;

    Sub(string id, EventBus *eventBus) : id(id), eventBus(eventBus) {}

    void subscribe_topic(const string &topicId)
    {
        eventBus->subscribe_topic(topicId, this);
    }

    void unsubscribe_topic(const string &topicId)
    {
        eventBus->unsubscribe_topic(topicId, this);
    }

    void event_callback(const string &event, const string &topicId)
    {
        cout << id << " Reporting event " << event << " pushed to " << topicId << endl;
    }
};

// Define push_event after Sub is fully defined.
void EventBus::push_event(const string &topicId, const string &event)
{
    // Record the event in the events history.
    events[topicId].push_back(event);
    // Notify each subscriber registered for this topic.
    for (auto sub : listeners[topicId])
    {
        sub->event_callback(event, topicId);
    }
}

int main()
{
    // Using stack allocation for simplicity
    EventBus eventBus;
    Pub pub("p1", &eventBus);
    Sub s1("s1", &eventBus);
    Sub s2("s2", &eventBus);
    Sub s3("s3", &eventBus);

    s1.subscribe_topic("t1");
    s2.subscribe_topic("t2");
    pub.addEvent("t1", "e1");
    s1.unsubscribe_topic("t1");
    s3.subscribe_topic("t2");
    pub.addEvent("t1", "e1.1");
    pub.addEvent("t2", "e2");

    return 0;
}
