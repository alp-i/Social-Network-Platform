#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Cengram.h"

extern Node *userProfiles;
extern int totalUserCount;
extern Node *allPosts;
extern int totalPostCount;


/*utility functions*/

User* castUser(Node* tmp){
    if(tmp==NULL)
        return NULL;

    return (User*)(tmp->data);
}

Post* castPost(Node* tmp){
    if(tmp==NULL)
        return NULL;

    return (Post*)(tmp->data);
}

void strAllocate(char **to,char *from){
    if(to==NULL)
        return;

    *to=(char*)malloc(sizeof(char)*(strlen(from)+1));
    strcpy(*to,from);
}

Node* getNode(int id){      /*returns Node* corresponding to the specific user id*/

    Node *curr=userProfiles;

    while(curr!=NULL){
        if((castUser(curr))->userId==id)
            return curr;
        curr=curr->next;
    }

    return  NULL;
}


void insertNode(Node **head,Node *package,int index){ /* generic insertion Node* head is passed (followers,posts,userprofiles) last index indication, 0 begining node */

    Node **curr=head;
    Node *tmp_next;
    int i;

    for(i=0;i<index;i++){
        curr=&((*curr)->next);
    }
    tmp_next=(*curr);

    *curr=package;
    package->next=tmp_next;
}

int findInsertPos_allposts(int postId){ /* returns the position to be inserted in ascend order in allposts*/
    Node *curr=allPosts;
    int i=0;

    while(curr!=NULL && postId>castPost(curr)->postId){
        i++;
        curr=curr->next;
    }

    return i;
}



/*assignment functions*/



User *registerUser(int newUserId, char *newUsername, int birthdayDay, int birthdayMonth, int birthdayYear)
{
    /* TODO: Implement this function */

    Node *package=(Node*)malloc(sizeof(Node));  /*Node creation*/

    User *nw=(User*)malloc(sizeof(User));         /*User creation*/

                                                /*User construction*/
    nw->userId=newUserId;
    strAllocate(&(nw->username),newUsername);   /*string malloc + strcpy*/
    nw->birthday=(Date*)malloc(sizeof(Date));
    nw->birthday->day=birthdayDay;
    nw->birthday->month=birthdayMonth;
    nw->birthday->year=birthdayYear;

    nw->followers=NULL;
    nw->posts=NULL;
    nw->numOfFollowers=0;
    nw->numOfPosts=0;



                                                /*Node construction*/
    package->next=NULL;
    package->data=nw;


    insertNode(&userProfiles,package,totalUserCount);         /* (Node + User) package insertion */
                                                /* totalusercount indicates the last element in the list*/

    totalUserCount++;  /*should be incremented here */

    return nw;
}


void followUser(int userId1, int userId2)
{
    /* TODO: Implement this function */

    Node *folwr=getNode(userId1);
    Node *folwd=getNode(userId2);
    Node *package=(Node*)malloc(sizeof(Node));      /*package creation*/


    if(folwr==NULL){ /*it's guranteed to exist in the userlist*/
        printf("User does not exist in the user list !");
        return;
    }



    package->data=castUser(folwr);      /*pack construction*/
    package->next=NULL;

    insertNode(&(castUser(folwd)->followers),package,castUser(folwd)->numOfFollowers);        /* User'ın num of follower ı kadar kayıp insert edince append etmiş olcaz*/

    (castUser(folwd)->numOfFollowers)++;            /*num of follower increment*/



}


int getPostId(Node **tmp){
    if(tmp==NULL || *tmp==NULL)
        return -9999;
    return ((Post*)((*tmp)->data))->postId;
}


Post *createPost(int postId, int ownerUserId, char *newContent)
{
    /* TODO: Implement this function */

    Post *pst=(Post*)malloc(sizeof(Post));  /*post creation*/

    Node *package=(Node*)malloc(sizeof(Node));  /*package creation*/
    Node *package_allposts=(Node*)malloc(sizeof(Node));

    Node *node_author=getNode(ownerUserId);
    User *user_author=castUser(node_author);

    /*post construction*/
    pst->author=user_author;
    pst->postId=postId;
    strAllocate(&(pst->content),newContent);

    /*package construction*/
    package->data=pst;
    package->next=NULL;

    /*insertion at the end of the posts numofposts indicates the end*/
    insertNode(&(user_author->posts),package,user_author->numOfPosts);

    /*num of posts in user incremented*/
    (user_author->numOfPosts)++;


    /*package_allposts construction*/
    package_allposts->data=pst;
    package_allposts->next=NULL;

    /*insertion to the global allposts in asc order*/
    insertNode(&allPosts,package_allposts,findInsertPos_allposts(postId));

    /*global totpostcount incremented for any insertion*/
    totalPostCount++;

    return pst;
}





void removeNode_Follower(User *usr,int followerID){     /*removes the follower with the id of the usr */

    Node **curr;
    Node *tmp;

    if(usr==NULL || usr->followers==NULL)
        return;
    curr=&(usr->followers);

    while(*curr!=NULL){
        if(castUser(*curr)->userId==followerID){
            tmp=*curr;
            (*curr)=(*curr)->next;
            tmp->data=NULL;
            tmp->next=NULL;
            free(tmp);

            (usr->numOfFollowers)--;
            if(usr->numOfFollowers==0)
                usr->followers=NULL;

        }

        if(*curr==NULL || (*curr)->next==NULL)
            return;

        curr=&((*curr)->next);
    }


}

void removeNode_Posts(User *usr,int userID){    /*removes the node post of userID in the user*/
    Node **curr;
    Node *tmp;

    if(usr==NULL || usr->posts==NULL)
        return;

    curr=&(usr->posts);

    while(*curr!=NULL){
        if(castPost(*curr)->author->userId==userID){
            tmp=*curr;
            (*curr)=(*curr)->next;
            tmp->data=NULL;
            tmp->next=NULL;
            free(tmp);

            (usr->numOfPosts)--;
            if(usr->numOfPosts==0)
                usr->posts=NULL;
        }

        if(*curr==NULL || (*curr)->next==NULL)
            return;

        curr=&((*curr)->next);
    }
}

void removeUser(Node *tmp){    /*removes the seperated user*/

    User *del=castUser(tmp);
    Node *folw=del->followers;
    Node *pst=del->posts;

    /*username, birthday deleted*/
    free(del->username);
    free(del->birthday);

    /*follower nodes deleted*/
    while(folw!=NULL){
        Node *rmv=folw;
        folw->data=NULL;
        folw=folw->next;
        free(rmv);
    }

    /*post nodes deleted*/
    while(pst!=NULL){
        Node *rmv=pst;
        pst->data=NULL;
        pst=pst->next;
        free(rmv);
    }

    /*nice convention freed ptrs set to null*/
    del->username=NULL;
    del->birthday=NULL;
    del->followers=NULL;
    del->posts=NULL;

    free(del);

    /*nice convention freed ptrs set to null*/
    tmp->data=NULL;
    tmp->next=NULL;

    free(tmp);

}

Node* seperateUser_userprofiles(int userID){
    Node **curr;
    Node *tmp;

    if(userProfiles==NULL)
        return NULL;

    curr=&userProfiles;


    while(*curr!=NULL){
        if(castUser(*curr)->userId==userID){
            tmp=*curr;
            *curr=(*curr)->next;
            return tmp; /*return the seperated node*/
        }

        if(*curr==NULL || (*curr)->next==NULL)
            return NULL;

        curr=&((*curr)->next);
    }

    return NULL;    /*in case it is not in the userprofiles <impossible>*/
}

void removeNode_allposts(int userID){
    Node **curr;
    Node *tmp;

    if(allPosts==NULL)
        return;
    curr=&allPosts;

    while(*curr!=NULL){
        if(castPost(*curr)->author->userId==userID){

            Post *rmv;
            tmp=*curr;

            /*post removed*/
            rmv=castPost(tmp);
            free(rmv->content);
            rmv->author=NULL;
            /*post node removed*/
            tmp->data=NULL;
            *curr=(*curr)->next;

            tmp->next=NULL;
            free(rmv);
            free(tmp);

            totalPostCount--;
            if(totalPostCount==0)
                allPosts=NULL;

            continue;
        }

        if(*curr==NULL || (*curr)->next==NULL)
            return;;

        curr=&((*curr)->next);
    }

}

void removeUserAccount(int deletedUserId)   /*burda kaldık remove yapılcak*/
{
    /* TODO: Implement this function */
    Node *tmp=seperateUser_userprofiles(deletedUserId);

    Node *curr=userProfiles;

    while(curr!=NULL){
        removeNode_Follower(castUser(curr),deletedUserId);
        removeNode_Posts(castUser(curr),deletedUserId);

        curr=curr->next;
    }

    removeNode_allposts(deletedUserId);

    removeUser(tmp);

    totalUserCount--;

}
