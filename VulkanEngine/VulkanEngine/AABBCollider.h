#pragma once

#include "Collider.h"

class AABBCollider : public Collider
{
private:
	glm::vec3 extents;

public:

#pragma region Accessors

	/// <summary>
	/// Returns the extents of the collider
	/// </summary>
	/// <returns>The collider's extents</returns>
	glm::vec3 GetExtents();

	/// <summary>
	/// Sets the extents to the specified value
	/// </summary>
	/// <param name="value">The value to set the extents to</param>
	void SetExtents(glm::vec3 value);

#pragma endregion

#pragma region Collision Detection

	/// <summary>
	/// Checks for a collision with another collider and returns true if there is one.
	/// </summary>
	/// <param name="other">The collider to check against</param>
	/// <returns>True if this collider is colliding with the other collider</returns>
	bool CheckCollision(Collider* other) override;

#pragma endregion

#pragma region Collider Generation

	/// <summary>
	/// Generates a collider from a mesh
	/// </summary>
	/// <param name="mesh"></param>
	void GenerateFromMesh(std::shared_ptr<Mesh> mesh) override;

#pragma endregion

#pragma Helper Methods

	/// <summary>
	/// Returns whether or not the specified point is in the collider
	/// </summary>
	/// <param name="point">The point to test</param>
	/// <returns>True if the specified point is inside of the collider</returns>
	bool ContainsPoint(glm::vec3 point) override;

	/// <summary>
	/// Returns the closest point on the collider to the specified point
	/// </summary>
	/// <param name="point">The point to check distance to</param>
	/// <returns>The point in the collider closest to the given point</returns>
	glm::vec3 ClosestToPoint(glm::vec3 point) override;

	/// <summary>
	/// Makes this collider visible or invisible to the user
	/// </summary>
	/// <param name="visible">Whether to show or hide the collider true to show</param>
	void ToggleVisible(bool visible) override;

#pragma endregion
};
