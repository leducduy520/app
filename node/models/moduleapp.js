// npm install express mongoose body-parser dotenv

const mongoose = require("mongoose");

// Define module schema
const moduleSchema = new mongoose.Schema(
  {
    module_id: { type: String, required: true },
    module_path: { type: String, required: true },
    execution_start: { type: String, required: false },
    execution_end: { type: String, required: false },
  },
  { _id: false }
);

// Define history schema
const historySchema = new mongoose.Schema(
  {
    sid: { type: mongoose.Schema.Types.ObjectId, required: true },
    modules: [moduleSchema], // Array of modules
  },
  { _id: false }
);

// Define the main schema with history as an array
const moduleappSchema = new mongoose.Schema(
  {
    _id: { type: String, required: true },
    history: [historySchema],
  },
  { versionKey: false } // Disable the __v field
);

/**
 * Export a function to dynamically create a model with the desired collection name.
 */
module.exports = (collectionName) => {
  return mongoose.model("ModuleApp", moduleappSchema, collectionName);
};
