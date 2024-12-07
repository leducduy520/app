const { URL } = require('url');

const AuthMethods = {
  X509: 'X509',
  PASSWORD: 'PASSWORD',
  NONE: 'NONE',
  INVALID: 'INVALID',
};

function detectAuthMethod(mongoUri) {
  try {
    const uri = new URL(mongoUri);

    // Check if an authentication mechanism is explicitly specified
    const authMechanism = uri.searchParams.get("authMechanism");
    if (authMechanism === "MONGODB-X509") {
      return AuthMethods.X509;
    }

    // Check if username and password are included in the URI
    const username = uri.username;
    const password = uri.password;
    if (username && password) {
      return AuthMethods.PASSWORD;
    }

    // Fallback for other cases
    return AuthMethods.NONE;
  } catch (error) {
    console.error("Invalid MongoDB URI:", error.message);
    return AuthMethods.INVALID;
  }
}

/**
 * Export a function to detect authentication method.
 */
module.exports = { detectAuthMethod, AuthMethods };;
