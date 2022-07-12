// Firebase configurations retrieved from Firebase console
const firebaseConfig = {
    apiKey: "AIzaSyAmiLVwBOF-1XJmorl2M71JR0GLkxgluQA",
    authDomain: "finalproject-b8ce7.firebaseapp.com",
    databaseURL: "https://finalproject-b8ce7-default-rtdb.europe-west1.firebasedatabase.app",
    projectId: "finalproject-b8ce7",
    storageBucket: "finalproject-b8ce7.appspot.com",
    messagingSenderId: "854712597772",
    appId: "1:854712597772:web:a90968164cec240df99d29"
};

// Initialize firebase
firebase.initializeApp(firebaseConfig);

// Make auth and database references
const auth = firebase.auth();
const db = firebase.database();

//Code Reference: https://firebase.google.com/docs/web/setup

