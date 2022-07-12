// listen for auth status changes
auth.onAuthStateChanged(user => {
    if (user) {
        setupUI(user);
        
    } else {
        setupUI();
    }
});

// login
const userLoginForm = document.getElementById('login-form');
userLoginForm.addEventListener('submit', (event) => {
    event.preventDefault();
    // get user info
    const email = userLoginForm['userEmail'].value;
    const password = userLoginForm['userPassword'].value;
    // log the user in
    auth.signInWithEmailAndPassword(email, password).then((cred) => {
            userLoginForm.reset();
            console.log(email);
        })
        .catch((error) => {
            const errorCode = error.code;
            const errorMessage = error.message;
            document.getElementById("error-message").innerHTML = errorMessage;
            console.log(errorMessage);
        });
});

// logout
const logoutBTN = document.querySelector('#logoutBTN');
logoutBTN.addEventListener('click', (event) => {
    event.preventDefault();
    auth.signOut();
});


//Code Reference: https://dev.to/maasak/sign-up-login-logout-users-with-firebase-authentication-3oa9

