using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Paddle : MonoBehaviour
{
    public float speed = 30f;
    public Rigidbody2D body;
    public Vector2 direction;
    public float maxAngle = 75f;
    private void Awake()
    {
        this.body = GetComponent<Rigidbody2D>();
    }

    private void Update()
    {
        if(Input.GetKey(KeyCode.A) || Input.GetKey(KeyCode.LeftArrow))
        {
            this.direction = Vector2.left;
        } else if(Input.GetKey(KeyCode.D) || Input.GetKey(KeyCode.RightArrow))
        {
            this.direction = Vector2.right;
        } else
        {
            this.direction = Vector2.zero;
        }
    }

    private void FixedUpdate()
    {
        if(this.direction != Vector2.zero)
        {
            this.body.AddForce(this.direction * this.speed);
        }
    }

    private void OnCollisionEnter2D(Collision2D collision)
    {
        Ball ball = collision.gameObject.GetComponent<Ball>();
        if (ball != null)
        {
            Vector3 paddlePos = this.transform.position;
            Vector2 contact = collision.GetContact(0).point;

            float offset = paddlePos.x - contact.x;
            float width = collision.otherCollider.bounds.size.x / 2;

            float currentAngle = Vector2.SignedAngle(Vector2.up, ball.body.velocity);
            float bounceAngle = offset / width * this.maxAngle;
            float newAngle = Mathf.Clamp(currentAngle + bounceAngle, -this.maxAngle, this.maxAngle);

            Quaternion rotation = Quaternion.AngleAxis(newAngle, Vector3.forward);
            ball.body.velocity = rotation * Vector2.up * ball.body.velocity.magnitude;
        }
    }
}
