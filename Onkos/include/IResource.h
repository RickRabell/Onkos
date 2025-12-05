#pragma once
#include "Prerequisites.h"

/**
 * @enum ResourceType
 * @brief Defines the specific type of an asset.
 */
enum class 
ResourceType {
	Unknown,    /**< Undefined or uninitialized type. */
	Model3D,    /**< 3D Geometry data (OBJ, FBX). */
	Texture,    /**< Image data for texturing. */
	Sound,      /**< Audio data. */
	Shader,     /**< Compiled or source shader code. */
	Material    /**< Material definitions. */
};

/**
 * @enum ResourceState
 * @brief Tracks the lifecycle state of a resource.
 */
enum class 
ResourceState {
	Unloaded,   /**< The resource is not in memory. */
	Loading,    /**< The resource is currently being loaded/parsed. */
	Loaded,     /**< The resource is fully loaded and ready for use. */
	Failed      /**< The resource failed to load. */
};

/**
 * @class IResource
 * @brief Abstract base class for all manageable assets in the engine.
 * @author Ricardo Rabell
 * @date 2025-11-14
 *
 * This interface defines the contract for any resource (Texture, Model, Sound).
 * It handles common metadata like the resource name, file path, unique ID,
 * and loading state. It mandates the implementation of initialization, loading,
 * and unloading logic.
 */
class 
IResource {
public:
	/**
	 * @brief Constructor.
	 * Sets the resource name and generates a unique runtime ID.
	 * @param name The unique name or identifier for this resource.
	 */
	IResource(const std::string& name)
						: m_name(name)
						, m_filePath("")
						, m_type(ResourceType::Unknown)
						, m_state(ResourceState::Unloaded)
						, m_id(GenerateID()) 
	{ }

	/**
	 * @brief Virtual destructor.
	 */
	virtual 
	~IResource() = default;

	/**
	 * @brief Pure virtual function to initialize the resource on the GPU/API side.
	 * @return bool True if initialization was successful.
	 */
	virtual bool 
	init() = 0;

	/**
	 * @brief Pure virtual function to load the resource data from the disk (CPU side).
	 * @param filename The path to the file to load.
	 * @return bool True if the file was found and parsed successfully.
	 */
	virtual bool 
	load(const std::string& filename) = 0;

	/**
	 * @brief Pure virtual function to release memory and resources.
	 */
	virtual void 
	unload() = 0;

	/**
	 * @brief Gets the size of the resource in memory.
	 * @note Used primarily for profiling and memory management.
	 * @return size_t The size in bytes.
	 */
	virtual size_t 
	getSizeInBytes() const = 0;

	/**
	 * @brief Sets the file system path of the resource.
	 * @param path The full or relative path string.
	 */
	void 
	SetPath(const std::string& path) { m_filePath = path; }

	/**
	 * @brief Sets the specific type of the resource.
	 * @param t The resource type enum.
	 */
	void
	SetType(ResourceType t) { m_type = t; }

	/**
	 * @brief Updates the current lifecycle state of the resource.
	 * @param s The new state enum.
	 */
	void 
	SetState(ResourceState s) { m_state = s; }

	/**
	 * @brief Gets the name of the resource.
	 * @return const std::string& The resource name.
	 */
	const std::string& 
	GetName() const { return m_name; }

	/**
	 * @brief Gets the file path of the resource.
	 * @return const std::string& The file path.
	 */
	const std::string& 
	GetPath() const { return m_filePath; }

	/**
	 * @brief Gets the type of the resource.
	 * @return ResourceType The type enum.
	 */
	ResourceType 
	GetType() const { return m_type; }

	/**
	 * @brief Gets the current state of the resource.
	 * @return ResourceState The state enum.
	 */
	ResourceState 
	GetState() const { return m_state; }

	/**
	 * @brief Gets the unique runtime ID of the resource.
	 * @return uint64_t The unique ID.
	 */
	uint64_t 
	GetID() const { return m_id; }

protected:
	std::string m_name;       /**< Human-readable name of the resource. */

	std::string m_filePath;   /**< Path to the source file. */

	ResourceType m_type;      /**< The specific type of this resource. */

	ResourceState m_state;    /**< Current loading/usage state. */

	uint64_t m_id;            /**< Unique identifier. */

private:
	/**
	 * @brief Generates a unique ID for every new resource instance.
	 * @return uint64_t A thread-safe static incrementing ID.
	 */
	static uint64_t 
	GenerateID() {
		static uint64_t nextID = 1;
		return nextID++;
	}
};