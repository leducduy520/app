# Use Node.js LTS as the base image
FROM node:18

# Set the working directory
WORKDIR /node

# Install dependencies
COPY node/package*.json /node/
RUN npm install

# Expose the application port
EXPOSE 3000

# Copy application code
COPY node/ /node/

CMD ["npm", "start"]
