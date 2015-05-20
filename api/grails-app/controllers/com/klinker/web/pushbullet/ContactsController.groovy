/*
 * Copyright (C) 2015 Jacob Klinker
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

package com.klinker.web.pushbullet

import grails.converters.JSON
import groovyx.net.http.ContentType
import groovyx.net.http.RESTClient

/**
 * Forwards information on contacts to Pushbullet through https://api.pushbullet.com/v2/contacts
 */
class ContactsController {

    static allowedMethods = [add:'DELETE']

    /**
     * Find a JSON of the current user object's contacts based off of authorization header access token. More
     * information can be found at https://docs.pushbullet.com/#contacts.
     *
     * @return JSON containing current user's contacts
     */
    def index() {
        def client = new RESTClient(PushbulletApi.BASE_URL)
        def resp

        if (request.get) {
            // GET request, returns all contacts
            resp = client.get(
                    path: 'v2/contacts',
                    headers: ['Authorization': request.getHeader('Authorization')],
                    requestContentType: ContentType.JSON
            )
        } else if (request.post) {
            def id = params.id

            if (id == null) {
                // POST request, creates a new contact
                resp = client.post(
                        path: 'v2/contacts',
                        headers: ['Authorization': request.getHeader('Authorization')],
                        requestContentType: ContentType.JSON,
                        body: [
                                name: params.name,
                                email: params.email
                        ]
                )
            } else {
                // POST request, updates a contact
                resp = client.post(
                        path: 'v2/contacts/' + id,
                        headers: ['Authorization': request.getHeader('Authorization')],
                        requestContentType: ContentType.JSON,
                        body: [
                                name: params.name
                        ]
                )
            }
        } else {
            // DELETE request, deletes a contact
            resp = client.delete(
                    path: 'v2/contacts/' + params.id,
                    headers: ['Authorization': request.getHeader('Authorization')],
                    requestContentType: ContentType.JSON
            )
        }

        assert resp.status == 200
        render resp.getData() as JSON
        return resp.getData()
    }
}