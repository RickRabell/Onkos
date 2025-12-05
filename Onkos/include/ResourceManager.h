#pragma once
#include "Prerequisites.h"
#include "IResource.h"

/**
 * @class ResourceManager
 * @brief A Singleton class responsible for managing the lifecycle of game assets.
 * @author Ricardo Rabell
 * @date 2025-11-20
 *
 * The Resource Manager implements a caching mechanism (acting as a Flyweight factory).
 * It ensures that resources (Models, Textures, Shaders) are loaded from disk only once.
 * Subsequent requests for the same resource return a shared pointer to the existing instance.
 */
class 
ResourceManager {
public:
	/**
	 * @brief Default constructor.
	 */
	ResourceManager() = default;

	/**
	 * @brief Default destructor.
	 */
	~ResourceManager() = default;

	/**
	 * @brief Retrieves the singleton instance of the Resource Manager.
	 * @return ResourceManager& Reference to the static instance.
	 */
	static ResourceManager& 
	getInstance() {
		static ResourceManager instance;
		return instance;
	}

	// Delete copy constructor and assignment operator to enforce Singleton pattern
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

	/**
	 * @brief Retrieves an existing resource or loads it from disk if not found.
	 *
	 * This template method checks the internal cache for a resource with the given key.
	 * If found, it casts it to type T and returns it. If not, it constructs, loads,
	 * initializes, and caches the new resource.
	 *
	 * @tparam T The specific resource type (must inherit from IResource).
	 * @tparam Args Variadic arguments forwarded to the resource's constructor.
	 * @param key The unique string identifier for the resource in the cache.
	 * @param filename The file path to load the resource from if it doesn't exist.
	 * @param args Additional arguments for the resource constructor.
	 * @return std::shared_ptr<T> A shared pointer to the requested resource, or nullptr on failure.
	 */
	template<typename T, typename... Args>
	std::shared_ptr<T> 
	GetOrLoad(const std::string& key,
						const std::string& filename,
						Args&&... args) {

		static_assert(std::is_base_of<IResource, T>::value, "T must inherit from IResource");

		// 1. Does the resource exist in the cache?
		auto it = m_resources.find(key);
		
		if (it != m_resources.end()) {
			// Attempt to cast to the correct type
			auto existing = std::dynamic_pointer_cast<T>(it->second);
			
			// Check if valid and fully loaded
			if (existing && existing->GetState() == ResourceState::Loaded) {
				return existing; // Flyweight: reuse the instance
			}
		}

		// 2. Does not exist or is not loaded -> create and load it
		std::shared_ptr<T> resource = std::make_shared<T>(key, std::forward<Args>(args)...);

		if (!resource->load(filename)) {
			// Error handling can be improved here
			return nullptr;
		}

		if (!resource->init()) {
			return nullptr;
		}

		// 3. Store in cache and return
		m_resources[key] = resource;
		return resource;
	}

	/**
	 * @brief Retrieves a resource that is expected to be already loaded.
	 * @tparam T The specific resource type.
	 * @param key The unique string identifier used during loading.
	 * @return std::shared_ptr<T> The resource if found, otherwise nullptr.
	 */
	template<typename T>
	std::shared_ptr<T> 
	Get(const std::string& key) const {
		auto it = m_resources.find(key);

		if (it == m_resources.end()) return nullptr;

		return std::dynamic_pointer_cast<T>(it->second);
	}

	/**
	 * @brief Unloads and removes a specific resource from the manager.
	 * @param key The unique string identifier of the resource to remove.
	 */
	void 
	Unload(const std::string& key) {
		auto it = m_resources.find(key);

		if (it != m_resources.end()) {
			it->second->unload();
			m_resources.erase(it);
		}
	}

	/**
	 * @brief Unloads all resources managed by this class.
	 * Should be called during application shutdown to prevent memory leaks.
	 */
	void 
	UnloadAll() {
		for (auto& [key, res] : m_resources) {
			if (res) {
				res->unload();
			}
		}

		m_resources.clear();
	}

private:
	/** @brief The hash map storing the resources, keyed by a string ID. */
	std::unordered_map<std::string, std::shared_ptr<IResource>> m_resources;
};